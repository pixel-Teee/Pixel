Texture2D<int> TextureInput : register(t0);

StructuredBuffer<float2> UVBuffer : register(t1);

RWStructuredBuffer<int> RWColorBuffer : register(u0);

SamplerState Sampler : register(s0);

cbuffer Information : register(b0)
{
	int width;
	int height;
};

[numthreads(32, 32, 1)]
void CSGetPixels(uint3 threadId : SV_DispatchThreadID)
{
	//float2 UV = UVBuffer[threadId.x].xy;

	int index = width * threadId.y + min(threadId.x, width);
	int color = TextureInput[int2(min(threadId.x, width), min(threadId.y, height))];
	RWColorBuffer[index] = color;
}