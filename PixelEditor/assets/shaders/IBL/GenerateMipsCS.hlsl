#define BLOCK_SIZE 8//size of the thread group in a single dimension

#define WIDTH_HEIGHT_EVEN 0
#define WIDTH_ODD_HEIGHT_EVEN 1
#define WIDTH_EVEN_HEIGHT_ODD 2
#define WIDTH_HEIGHT_ODD 3

struct ComputeShaderInput
{
	uint3 GroupID : SV_GroupID;
	uint3 GroupThreadID : SV_GroupThreadID;
	uint3 DispatchThreadID : SV_DispatchThreadID;
	uint GroupIndex : SV_GroupIndex;
};

/*------
SrcDimension

BITMASK DESCRIPTION
0b00	Both width and height are even
0b01	Width is odd, height is even
0b10	Width is even, height is odd
0b11	Both width and height are odd
-------*/

cbuffer GenerateMipsCB : register(b0)
{
	uint SrcMipLevel;//texture level of source mips
	uint NumMipLevels;//number of outmips to write:[1-4]
	uint SrcDimension;//width and height of the source texture are even or odd
	bool IsSRGB;//must apply gamma correction to sRGB textures
	float2 TexelSize;//1.0 / OutMip1.Dimensions
}

//source mip map
Texture2D<float4> SrcMip : register(t0);

//write up to 4 mip map levels
RWTexture2D<float4> OutMip1 : register(u0);
RWTexture2D<float4> OutMip2 : register(u1);
RWTexture2D<float4> OutMip3 : register(u2);
RWTexture2D<float4> OutMip4 : register(u3);

//linear clamp sampler
SamplerState LinearClampSampler : register(s0);

groupshared float gs_R[64];//32bit * 64
groupshared float gs_G[64];
groupshared float gs_B[64];
groupshared float gs_A[64];

void StoreColor(uint Index, float4 Color)
{
	gs_R[Index] = Color.r;
	gs_G[Index] = Color.g;
	gs_B[Index] = Color.b;
	gs_A[Index] = Color.a;
}

float4 LoadColor(uint Index)
{
	return float4(gs_R[Index], gs_G[Index], gs_B[Index], gs_A[Index]);
}

//convert to linear
float4 ConvertToLinear(float3 x)
{
	return x < 0.04045f ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}

float3 ConvertToSRGB(float3 x)
{
	return x < 0.0031308 ? 12.92 * x : 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055;
}

//conver linear color to sRGB before stroing if the original source is an sRGB texture
float4 PackColor(float4 x)
{
	if (IsSRGB)
	{
		return float4(ConvertToSRGB(x.rgb), x.a);
	}
	else
	{
		return x;
	}
}

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CS(ComputeShaderInput IN)
{
	float4 Src1 = (float4)0;//store the color value of the mip level currently beging generated
	switch (SrcDimension)
	{
		case WIDTH_HEIGHT_EVEN:
		{
			float2 UV = TexelSize * (IN.DispatchThreadID.xy + 0.5f);

			Src1 = SrcMip.SampleLevel(LinearClampSampler, UV, SrcMipLevel);
		}
		break;
		case WIDTH_ODD_HEIGHT_EVEN:
		{
			float2 UV1 = TexelSize * (IN.DispatchThreadID.xy + float2(0.25f, 0.5f));
			float2 Off = TexelSize * float2(0.5f, 0.0f);//u axis need multiple texel

			Src1 = 0.5f * (SrcMip.SampleLevel(LinearClampSampler, UV1, SrcMipLevel)
				+ SrcMip.SampleLevel(LinearClampSampler, UV1 + Off, SrcMipLevel));
		}
		break;
		case WIDTH_EVEN_HEIGHT_ODD:
		{
			float2 UV1 = TexelSize * (IN.DispatchThreadID.xy + float2(0.5, 0.25));
			float2 Off = TexelSize * float2(0.0, 0.5);

			Src1 = 0.5 * (SrcMip.SampleLevel(LinearClampSampler, UV1, SrcMipLevel) +
					SrcMip.SampleLevel(LinearClampSampler, UV1 + Off, SrcMipLevel));
		}
		break;
		case WIDTH_HEIGHT_ODD:
		{
			float2 UV1 = TexelSize * (IN.DispatchThreadID.xy + float2(0.25, 0.25));
			float2 Off = TexelSize * 0.5;

			Src1 = SrcMip.SampleLevel(LinearClampSampler, UV1, SrcMipLevel);
			Src1 += SrcMip.SampleLevel(LinearClampSampler, UV1 + float2(Off.x, 0.0), SrcMipLevel);
			Src1 += SrcMip.SampleLevel(LinearClampSampler, UV1 + float2(0.0, Off.y), SrcMipLevel);
			Src1 += SrcMip.SampleLevel(LinearClampSampler, UV1 + float2(Off.x, Off.y), SrcMipLevel);
			Src1 *= 0.25;
		}
		break;
	}

	//written to first destination mip
	OutMip1[IN.DispathThreadID.xy] = PackColor(Src1);

	if (NumMipLevels == 1)
		return;

	StoreColor(IN.GroupIndex, Src1);

	GroupMemoryBarrierWithGroupSync();

	//with low three bits for x and high three bits for y
	//this bit mask checks that x and y are even
	if ((IN.GroupIndex & 0x9) == 0)
	{
		float4 Src2 = LoadColor(IN.GroupIndex + 0x01);
		float4 Src3 = LoadColor(IN.GroupIndex + 0x08);
		float4 Src4 = LoadColor(IN.GroupIndex + 0.09);
		Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

		OutMip2[IN.DispatchThreadID.xy / 2] = PackColor(Src1);
		StoreColor(IN.GroupIndex, Src1);
	}

	if (NumMipLevels == 2)
		return;

	GroupMemoryBarrierWithGroupSync();

	if ((IN.GroupIndex & 0x1B) == 0)
	{
		float4 Src2 = LoadColor(IN.GroupIndex + 0x02);
		float4 Src3 = LoadColor(IN.GroupIndex + 0x10);
		float4 Src4 = LoadColor(IN.GroupIndex + 0x12);
		Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

		OutMip3[IN.DispatchThreadID.xy / 4] = PackColor(Src1);
		StoreColor(IN.GroupIndex, Src1);
	}

	if (NumMipLevels == 3)
		return;

	GroupMemoryBarrierWithGroupSync();

	if (IN.GroupIndex == 0)
	{
		float4 Src2 = LoadColor(IN.GroupIndex + 0x04);
		float4 Src3 = LoadColor(IN.GroupIndex + 0x20);
		float4 Src4 = LoadColor(IN.GroupIndex + 0x24);
		Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

		OutMip4[IN.DispatchThreadID.xy / 8] = PackColor(Src1);
	}
}