Texture2D<float4> TextureInput : register(t0);

StructuredBuffer<float2> UVBuffer : register(t1);

RWStructuredBuffer<float4> RWColorBuffer : register(u0);

SamplerState Sampler : register(s0);

[numthreads(256, 1, 1)]
void CSGetPixels(uint3 threadId : SV_DispatchThreadID)
{
	float4 color = TextureInput.SampleLevel(Sampler, UVBuffer[threadId.x].xy, 0);
	RWColorBuffer[threadId.x] = color;
}