#include "../Common/Common.hlsl"
cbuffer CbMaterial : register(b2)
{
float4  a;
float time;
bool HaveNormal;
int ShadingModelID;
float ClearCoat;
float ClearCoatRoughness;
};

SamplerState gsamPointWrap : register(s0);
PixelOut PS(VertexOut pin){
float4  ConstFloatValue159;
ConstFloatValue159 = float4(0.000000, 0.000000, 0.000000, 0.000000);
float2  TexCoordinate14 = float2(0, 0);
TexCoordinate14 = pin.TexCoord;
float4  ConstFloatValue101;
ConstFloatValue101 = float4(12.560000, 12.560000, 0.000000, 1.000000);
float4  MulInputA22 = float4(TexCoordinate14.x, TexCoordinate14.y, TexCoordinate14.y, TexCoordinate14.y);
float4  MulInputB23 = ConstFloatValue101;
float4  MulOutput24 = float4(0, 0, 0, 1);
MulOutput24 = MulInputA22 * MulInputB23;
float4  SinInputValue110 = MulOutput24;
float4  SinOutputValue111 = float4(0, 0, 0, 1);
SinOutputValue111 = sin(SinInputValue110);
float4  InputValue116 = SinOutputValue111;
float  OutputValue117 = 0;
OutputValue117 = InputValue116.r;
float4  InputValue121 = SinOutputValue111;
float  OutputValue122 = 0;
OutputValue122 = InputValue121.g;
float  DotA51 = OutputValue117;
float  DotB52 = OutputValue122;
float4  Scalar53 = float4(0, 0, 0, 1);
Scalar53 = dot(DotA51, DotB52);
float  InputValue60 = Scalar53.x;
float  CeilOutput61 = 0;
CeilOutput61 = ceil(InputValue60);
float4  X66 = ConstFloatValue159;
float4  Y67 = a;
float4  S68 = float4(CeilOutput61, CeilOutput61, CeilOutput61, CeilOutput61);
float4  LerpOutput69 = float4(0, 0, 0, 1);
LerpOutput69 = lerp(X66, Y67, S68);
float4  ConstFloatValue130;
ConstFloatValue130 = float4(1.000000, 0.000000, 0.000000, 1.000000);
float4  Normal = float4(0, 0, 0, 1);
float4  Albedo = LerpOutput69;
float  Roughness = ConstFloatValue130.x;
float  Metallic = ConstFloatValue130.y;
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