//decode normal map
float3 DecodeNormalMap(float2 uv, float3 worldPos, float3 normal)
{
	float3 TangentNormal = gNormalMap.Sample(gsamPointWrap, uv).xyz * 2.0f - 1.0f;

	float3 Q1 = ddx(worldPos);
	float3 Q2 = ddy(worldPos);
	float2 st1 = ddx(uv);
	float2 st2 = ddy(uv);

	float3 N = normalize(normal);
	float3 T = normalize(Q1 * st2.y - Q2 * st1.y);
	float3 B = -normalize(cross(N, T));
	float3x3 TBN = float3x3(T, B, N);

	//let the texture map's normal to world's normal
	return normalize(mul(TangentNormal, TBN));
}