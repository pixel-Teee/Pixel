#include "../Common/Common.hlsl"
cbuffer CbMaterial : register(b2)
{
float4  a;
bool HaveNormal;
int ShadingModelID;
float ClearCoat;
float ClearCoatRoughness;
};

SamplerState gsamPointWrap : register(s0);
PixelOut PS(VertexOut pin){
float4  ConstFloatValue148;
ConstFloatValue148 = float4(0.000000, 0.000000, 0.000000, 1.000000);
float4  ConstFloatValue125;
ConstFloatValue125 = float4(25.120001, 25.120001, 0.000000, 1.000000);
float2  TexCoordinate15 = float2(0, 0);
TexCoordinate15 = pin.TexCoord;
float4  MulInputA18 = ConstFloatValue125;
float4  MulInputB19 = float4(TexCoordinate15.x, TexCoordinate15.y, TexCoordinate15.y, TexCoordinate15.y);
float4  MulOutput20 = float4(0, 0, 0, 1);
MulOutput20 = MulInputA18 * MulInputB19;
float4  SinInputValue27 = MulOutput20;
float4  SinOutputValue28 = float4(0, 0, 0, 1);
SinOutputValue28 = sin(SinInputValue27);
float4  InputValue33 = SinOutputValue28;
float  OutputValue34 = 0;
OutputValue34 = InputValue33.r;
float4  InputValue38 = SinOutputValue28;
float  OutputValue39 = 0;
OutputValue39 = InputValue38.g;
float  DotA45 = OutputValue34;
float  DotB46 = OutputValue39;
float4  Scalar47 = float4(0, 0, 0, 1);
Scalar47 = dot(DotA45, DotB46);
float  InputValue80 = Scalar47.x;
float  CeilOutput81 = 0;
CeilOutput81 = ceil(InputValue80);
float4  X86 = ConstFloatValue148;
float4  Y87 = a;
float4  S88 = float4(CeilOutput81, CeilOutput81, CeilOutput81, CeilOutput81);
float4  LerpOutput89 = float4(0, 0, 0, 1);
LerpOutput89 = lerp(X86, Y87, S88);
float4  ConstFloatValue61;
ConstFloatValue61 = float4(0.000000, 1.000000, 0.000000, 1.000000);
float4  Normal = float4(0, 0, 0, 1);
float4  Albedo = LerpOutput89;
float  Roughness = ConstFloatValue61.y;
float  Metallic = ConstFloatValue61.x;
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