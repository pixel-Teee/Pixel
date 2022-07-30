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

	//HDR Color To LDR
	float3 mappedNowColor = nowColor / (nowColor + float3(1.0f, 1.0f, 1.0f));

	//find closest velocity
	float2 velocity = velocityTexture.Sample(gsamPointWrap, getClosestOffset(pin.TexCoord)).xy;
	float2 offsetUV = clamp(pin.TexCoord - velocity, 0.0f, 1.0f);
	float3 preColor = previousScene.Sample(gsamPointWrap, offsetUV).xyz;

	float3 mappedPreColor = preColor / (preColor + float3(1.0f, 1.0f, 1.0f));

	//blend
	float c = 0.05f;

	float3 resultColor = c * mappedNowColor + (1 - c) * mappedPreColor;

	//LDR TO HDR Color
	//inverse tone mapping
	
	//taa
	pixelOut.Color = float4(resultColor / max((float3(1.0f, 1.0f, 1.0f) - resultColor), 0.001f), 1.0f);

	return pixelOut;
}