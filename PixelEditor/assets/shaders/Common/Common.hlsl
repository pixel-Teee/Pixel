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