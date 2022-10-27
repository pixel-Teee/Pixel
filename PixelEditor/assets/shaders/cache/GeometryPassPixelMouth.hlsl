#include "../Common/Common.hlsl"
cbuffer CbMaterial : register(b2)
{
float4  a;
bool HaveNormal;
int ShadingModelID;
float ClearCoat;
float ClearCoatRoughness;
};
Texture2D BaseColor : register(t0);
Texture2D MetallicRoughness : register(t1);
Texture2D Normal : register(t2);

SamplerState gsamPointWrap : register(s0);
PixelOut PS(VertexOut pin){
float2  Texture2DCoordinateInput25 = pin.TexCoord;
float4  Texture2DOutput26 = float4(0, 0, 0, 0);
Texture2DOutput26 = Normal.Sample(gsamPointWrap, Texture2DCoordinateInput25);
float2  Texture2DCoordinateInput7 = pin.TexCoord;
float4  Texture2DOutput8 = float4(0, 0, 0, 0);
Texture2DOutput8 = BaseColor.Sample(gsamPointWrap, Texture2DCoordinateInput7);
float4  MulInputA43 = a;
float4  MulInputB44 = Texture2DOutput8;
float4  MulOutput44 = float4(0, 0, 0, 1);
MulOutput44 = MulInputA43 * MulInputB44;
float2  Texture2DCoordinateInput16 = pin.TexCoord;
float4  Texture2DOutput17 = float4(0, 0, 0, 0);
Texture2DOutput17 = MetallicRoughness.Sample(gsamPointWrap, Texture2DCoordinateInput16);
float4  Normal = Texture2DOutput26;
float4  Albedo = MulOutput44;
float  Roughness = Texture2DOutput17.y;
float  Metallic = Texture2DOutput17.x;
float  Ao = 0;
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