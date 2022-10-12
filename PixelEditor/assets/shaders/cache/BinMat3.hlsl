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
return pixelOut;
}