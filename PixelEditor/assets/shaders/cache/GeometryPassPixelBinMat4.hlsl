#include "../Common/Common.hlsl"
cbuffer CbMaterial : register(b2)
{
bool HaveNormal;
int ShadingModelID;
float ClearCoat;
float ClearCoatRoughness;
};
Texture2D MyNormal : register(t0);
Texture2D MyColor : register(t1);
Texture2D MetallicRoughness : register(t2);

SamplerState gsamPointWrap : register(s0);
PixelOut PS(VertexOut pin){
float2  Texture2DCoordinateInput7 = pin.TexCoord;
float4  Texture2DOutput8 = float4(0, 0, 0, 0);
#if HAVE_NORMAL > 0
Texture2DOutput8 = DecodeNormalMap(pin.NormalW, pin.TangentW, MyNormal.Sample(gsamPointWrap, Texture2DCoordinateInput7) * 2.0f - 1.0f);
#else
Texture2DOutput8 = MyNormal.Sample(gsamPointWrap, Texture2DCoordinateInput7);
#endif
float2  Texture2DCoordinateInput17 = pin.TexCoord;
float4  Texture2DOutput18 = float4(0, 0, 0, 0);
Texture2DOutput18 = MyColor.Sample(gsamPointWrap, Texture2DCoordinateInput17);
float2  Texture2DCoordinateInput27 = pin.TexCoord;
float4  Texture2DOutput28 = float4(0, 0, 0, 0);
Texture2DOutput28 = MetallicRoughness.Sample(gsamPointWrap, Texture2DCoordinateInput27);
float4  Normal = Texture2DOutput8;
float4  Albedo = Texture2DOutput18;
float  Roughness = Texture2DOutput28.y;
float  Metallic = Texture2DOutput28.x;
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