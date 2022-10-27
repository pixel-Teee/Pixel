#include "../Common/Common.hlsl"
cbuffer CbMaterial : register(b2)
{
float4  a;
float4  b;
float4  c;
bool HaveNormal;
int ShadingModelID;
float ClearCoat;
float ClearCoatRoughness;
};
Texture2D MyTexture : register(t0);

SamplerState gsamPointWrap : register(s0);
PixelOut PS(VertexOut pin){
float4  MulInputA44 = a;
float4  MulInputB45 = b;
float4  MulOutput45 = float4(0, 0, 0, 1);
MulOutput45 = MulInputA44 * MulInputB45;
float2  Texture2DCoordinateInput52 = pin.TexCoord;
float4  Texture2DOutput53 = float4(0, 0, 0, 0);
Texture2DOutput53 = MyTexture.Sample(gsamPointWrap, Texture2DCoordinateInput52);
float4  MulInputA61 = MulOutput45;
float4  MulInputB62 = Texture2DOutput53;
float4  MulOutput62 = float4(0, 0, 0, 1);
MulOutput62 = MulInputA61 * MulInputB62;
float4  Normal = float4(0, 0, 0, 1);
float4  Albedo = MulOutput62;
float  Roughness = c.x;
float  Metallic = 0;
float  Ao = 0;
Normal = float4(pin.NormalW, 1.0f);
PixelOut pixelOut = (PixelOut)(0.0f);
pixelOut.gBufferPosition.xyz = pin.PosW;
pixelOut.gBufferNormal.xyz = (Normal.xyz + 1.0f) / 2.0f;
pixelOut.gBufferAlbedo.xyz = Albedo.xyz;
pixelOut.gBufferAlbedo.w = ClearCoatRoughness;
pixelOut.gVelocity.w = ClearCoat;
pixelOut.gBufferRoughnessMetallicAo.x = Roughness;
pixelOut.gBufferRoughnessMetallicAo.y = Metallic;
pixelOut.gBufferRoughnessMetallicAo.z = Ao;
pixelOut.gBufferRoughnessMetallicAo.w = ShadingModelID / 255.0f;
float2 newPos = ((pin.nowScreenPosition.xy / pin.nowScreenPosition.w) * 0.5f + 0.5f);
float2 prePos = ((pin.preScreenPosition.xy / pin.preScreenPosition.w) * 0.5f + 0.5f);
pixelOut.gVelocity.xy = newPos - prePos;
pixelOut.gVelocity.z = 0.0f;
return pixelOut;
}