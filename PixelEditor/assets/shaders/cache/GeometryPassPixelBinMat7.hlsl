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
float4  ConstFloatValue100;
ConstFloatValue100 = float4(0.000000, 0.000000, 0.000000, 1.000000);
float2  TexCoordinate6 = float2(0, 0);
TexCoordinate6 = pin.TexCoord;
float4  ConstFloatValue22;
ConstFloatValue22 = float4(50.240002, 50.240002, 0.000000, 0.000000);
float4  MulInputA15 = float4(TexCoordinate6.x, TexCoordinate6.y, TexCoordinate6.y, TexCoordinate6.y);
float4  MulInputB16 = ConstFloatValue22;
float4  MulOutput17 = float4(0, 0, 0, 1);
MulOutput17 = MulInputA15 * MulInputB16;
float4  SinInputValue32 = MulOutput17;
float4  SinOutputValue33 = float4(0, 0, 0, 1);
SinOutputValue33 = sin(SinInputValue32);
float4  InputValue38 = SinOutputValue33;
float  OutputValue39 = 0;
OutputValue39 = InputValue38.r;
float4  InputValue44 = SinOutputValue33;
float  OutputValue45 = 0;
OutputValue45 = InputValue44.g;
float  DotA55 = OutputValue39;
float  DotB56 = OutputValue45;
float4  Scalar57 = float4(0, 0, 0, 1);
Scalar57 = dot(DotA55, DotB56);
float  InputValue64 = Scalar57.x;
float  CeilOutput65 = 0;
CeilOutput65 = ceil(InputValue64);
float4  X70 = ConstFloatValue100;
float4  Y71 = a;
float4  S72 = float4(CeilOutput65, CeilOutput65, CeilOutput65, CeilOutput65);
float4  LerpOutput73 = float4(0, 0, 0, 1);
LerpOutput73 = lerp(X70, Y71, S72);
float4  ConstFloatValue110;
ConstFloatValue110 = float4(1.000000, 0.000000, 0.000000, 1.000000);
float4  Normal = float4(0, 0, 0, 1);
float4  Albedo = LerpOutput73;
float  Roughness = ConstFloatValue110.x;
float  Metallic = ConstFloatValue110.y;
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