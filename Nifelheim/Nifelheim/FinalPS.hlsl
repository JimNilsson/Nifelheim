struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

SamplerState AniSam : register(s0);

Texture2D Diffuse : register(t0);
Texture2D Normal : register(t1);
Texture2D Metallic : register(t2);
Texture2D Roughness : register(t3);
Texture2D Depth : register(t4);

float4 main(VS_OUT input) : SV_TARGET
{
	return Diffuse.Sample(AniSam,input.tex);
}