//------mesh constants------
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;//world matrix
	float4x4 ginvWorld;//inverse world matrix
	float4x4 previousWorld;//use for TAA
	int gEditor;
};
//------mesh constants------

cbuffer cbPass : register(b1)
{
	float4x4 gViewProjection;
	float3	camPos;//camera pos
	float gWidth;//width
};

cbuffer CbMaterial : register(b2)
{
	float3 gAlbedo;
	float gRoughness;
	float gMetallic;
	float gEmissive;
	bool HaveNormal;//have normal
	int ShadingModelID;//shading model id
};

struct VertexIn
{
	float3 PosL : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalL : NORMAL;
	//int Editor : EDITOR;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalW : NORMAL;
	int Editor : EDITOR;
};

struct PixelOut
{
	float4 gBufferPosition : SV_Target;
	float4 gBufferNormal : SV_Target1;
	float4 gVelocity : SV_Target2;
	float4 gBufferAlbedo : SV_Target3;
	float4 gBufferRoughnessMetallicEmissive : SV_Target4;
	int gEditor : SV_Target5;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	vout.NormalW = mul(vin.NormalL, (float3x3)transpose(ginvWorld));

	//to world space
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	float camPosToWorldPosLength = length(camPos - posW);

	//offset outline
	posW.xyz += vout.NormalW * camPosToWorldPosLength / max(gWidth, 0.01f) * 2.0f;

	vout.PosW = posW.xyz;

	//homogeneous clipping
	//vout.PosH = mul(posW, gView);
	vout.PosH = mul(posW, gViewProjection);

	vout.TexCoord = vin.TexCoord;

	vout.Editor = gEditor;
	return vout;
}

PixelOut PS(VertexOut pin)
{
	//write out to gbuffer
	PixelOut pixelOut = (PixelOut)(0.0f);
	pixelOut.gBufferPosition.xyz = pin.PosW;

	//if don't have normal map, then use the vertex's normal
	pixelOut.gBufferNormal.xyz = float3(-1.0f, -1.0f, -1.0f);

	pixelOut.gBufferAlbedo.xyz = float3(1.0f, 1.0f, 0.0f);

	float3 viewDir = normalize(camPos - pin.PosW);
	int FrontOrBackFace = sign(dot(viewDir, normalize(pin.NormalW.xyz)));//check back of front face

	pixelOut.gBufferAlbedo.w = FrontOrBackFace * -1.0f;
	pixelOut.gBufferRoughnessMetallicEmissive.x = 0.0f;
	pixelOut.gBufferRoughnessMetallicEmissive.y = 1.0f;
	pixelOut.gBufferRoughnessMetallicEmissive.z = 1.0f;
	pixelOut.gBufferRoughnessMetallicEmissive.w = ShadingModelID / 255.0f;
	pixelOut.gEditor = pin.Editor;

	return pixelOut;
}