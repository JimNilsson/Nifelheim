struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD;
};

TextureCube SkyMap;
SamplerState AniSam : register(s0);

float4 main(VS_OUT input) : SV_TARGET0
{
	return SkyMap.Sample(AniSam, input.tex.xyz);
}