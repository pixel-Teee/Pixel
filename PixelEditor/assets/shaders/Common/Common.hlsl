struct VertexIn
{
	float3 PosL : POSITION;
#if HAVE_TEXCOORD > 0
	float2 TexCoord : TEXCOORD;
#endif
#if HAVE_NORMAL > 0
	float3 NormalL : NORMAL;
#endif
#if HAVE_TANGENT > 0
	float3 TangentL : TANGENT;
#endif
#if HAVE_BINORMAL > 0
	float3 BinormalL : BINORMAL;
#endif
	//int Editor : EDITOR;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
#if HAVE_TEXCOORD > 0
	float2 TexCoord : TEXCOORD;
#endif
#if HAVE_NORMAL > 0
	float3 NormalW : NORMAL;
#endif
#if HAVE_TANGENT > 0
	float3 TangentW : TANGENT;
#endif
#if HAVE_BINORMAL > 0
	float3 BinormalW : BINORMAL;
#endif
	int Editor : EDITOR;

	//------use for TAA------
	float4 preScreenPosition : POSITION2;
	float4 nowScreenPosition : POSITION3;
	//------use for TAA------
};

struct PixelOut
{
	float4 gBufferPosition : SV_Target;
	float4 gBufferNormal : SV_Target1;
	float4 gVelocity : SV_Target2;
	float4 gBufferAlbedo : SV_Target3;
	float4 gBufferRoughnessMetallicAo : SV_Target4;
	int gEditor : SV_Target5;
};

float4 DecodeNormalMap(float3 NormalW, float3 TangentW, float3 BumpNormal)
{
	float3 N = normalize(NormalW);
	float3 T = TangentW;//
	float3 B = normalize(cross(N, T));
	float3x3 TBN = float3x3(T, B, N);

	//{T.x, T.y, T.z}
	//{B.x, B.y, B.z}
	//{N.x, N.y, N.z}

	return float4(normalize(mul(BumpNormal, TBN)), 1.0f);
}