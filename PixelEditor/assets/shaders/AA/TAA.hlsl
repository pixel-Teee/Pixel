cbuffer TAA : register(b1)//common cbv
{
	float gWidth;
	float gHeight;
	uint frameCount;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PixelOut
{
	float4 Color : SV_Target;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//to clip space
	vout.PosH = float4(vin.PosL, 1.0f);
	vout.TexCoord = vin.TexCoord;

	return vout;
}

Texture2D currentScene : register(t0);
Texture2D previousScene : register(t1);
Texture2D velocityTexture : register(t2);//velocity texture
Texture2D currentDepth : register(t3);

SamplerState gsamPointWrap : register(s0);

float3 RGB2YCoCgR(float3 rgbColor)
{
	float3 YCoCgRColor;

	YCoCgRColor.y = rgbColor.r - rgbColor.b;
	float temp = rgbColor.b + YCoCgRColor.y / 2.0f;
	YCoCgRColor.z = rgbColor.g - temp;
	YCoCgRColor.x = temp + YCoCgRColor.z / 2.0f;

	return YCoCgRColor;
}

float3 YCoCgR2RGB(float3 YCoCgRColor)
{
	float3 rgbColor;

	float temp = YCoCgRColor.x - YCoCgRColor.z / 2.0f;
	rgbColor.g = YCoCgRColor.z + temp;
	rgbColor.b = temp - YCoCgRColor.y / 2.0f;
	rgbColor.r = rgbColor.b + YCoCgRColor.y;

	return rgbColor;
}

float Luminance(float3 color)
{
	return 0.25 * color.r + 0.5 * color.g + 0.25 * color.b;
}

float3 ToneMap(float3 color)
{
	return color / (1 + Luminance(color));
}

float3 UnToneMap(float3 color)
{
	return color / (1 - Luminance(color));
}

float3 clipAABB(float3 nowColor, float3 preColor, float2 UV)
{
	float3 aabbMin = nowColor, aabbMax = nowColor;
	float2 deltaRes = float2(1.0f / gWidth, 1.0f / gHeight);
	float3 m1 = float3(0.0f, 0.0f, 0.0f), m2 = float3(0.0f, 0.0f, 0.0f);

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			float2 newUV = UV + deltaRes * float2(i, j);
			float3 C = RGB2YCoCgR(ToneMap(currentScene.Sample(gsamPointWrap, newUV).xyz));
			m1 += C;
			m2 += C * C;
		}
	}

	//Variance clip
	int N = 9;
	float VarianceClipGamma = 1.0f;
	float3 mu = m1 / N;
	float3 sigma = sqrt(abs(m2 / N - mu * mu));

	aabbMin = mu - VarianceClipGamma * sigma;
	aabbMax = mu + VarianceClipGamma * sigma;

	// clip to center
	float3 p_clip = 0.5 * (aabbMax + aabbMin);
	float3 e_clip = 0.5 * (aabbMax - aabbMin);

	float3 v_clip = preColor - p_clip;
	float3 v_unit = v_clip.xyz / e_clip;
	float3 a_unit = abs(v_unit);
	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

	if (ma_unit > 1.0)
		return p_clip + v_clip / ma_unit;
	else
		return preColor;
}

float2 getClosestOffset(float2 UV)
{
	float2 deltaRes = float2(1.0f / gWidth, 1.0f / gHeight);
	float closestDepth = 1.0f;
	float2 closestUV = UV;

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			float2 newUV = UV + deltaRes * float2(i, j);

			float depth = currentDepth.Sample(gsamPointWrap, newUV).x;

			if (depth < closestDepth)
			{
				closestDepth = depth;
				closestUV = newUV;
			}
		}
	}

	return closestUV;
}

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.0f);

	float3 nowColor = currentScene.Sample(gsamPointWrap, pin.TexCoord).xyz;

	//find closest velocity
	float2 velocity = velocityTexture.Sample(gsamPointWrap, getClosestOffset(pin.TexCoord)).xy;
	float2 offsetUV = clamp(pin.TexCoord - velocity, 0.0f, 1.0f);
	float3 preColor = previousScene.Sample(gsamPointWrap, offsetUV).xyz;

	//HDR TO LDR
	float3 mappedNowColor = RGB2YCoCgR(ToneMap(nowColor));
	float3 mappedPreColor = RGB2YCoCgR(ToneMap(preColor));

	mappedPreColor = clipAABB(mappedNowColor, mappedPreColor, pin.TexCoord);

	//LDR TO HDR
	mappedNowColor = UnToneMap(YCoCgR2RGB(mappedNowColor));
	mappedPreColor = UnToneMap(YCoCgR2RGB(mappedPreColor));

	//blend
	float c = 0.10f;

	float3 resultColor = c * mappedNowColor + (1 - c) * mappedPreColor;

	//taa
	pixelOut.Color = float4(resultColor.xyz, 1.0f);

	return pixelOut;
}