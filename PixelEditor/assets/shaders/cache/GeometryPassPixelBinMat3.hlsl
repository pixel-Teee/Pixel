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

PixelOut PS(VertexOut pin){
float4  MulInputA23 = a;
float4  MulInputB24 = b;
float4  MulOutput24 = float4(0, 0, 0, 1);
MulOutput24 = MulInputA23 * MulInputB24;
float4  Normal = float4(0, 0, 0, 1);
float4  Albedo = MulOutput24;
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