struct VS_OUT
{
	float4 posH : SV_POSITION;
	float4 posVS : POSITION;
	float3 toEye : NORMAL;
	float2 tex : TEXCOORD;
	float3x3 tbn : TBNMATRIX;
};

cbuffer PerFrameBuffer : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gViewProj;
	float4x4 gInvView;
	float4x4 gInvViewProj;
	float4 gCamPos;
}

struct PS_OUT
{
	float4 Diffuse : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D Roughness : register(t2);
Texture2D Metallic : register(t3);

SamplerState AniSam : register(s0);

PS_OUT main(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	//output.Diffuse = float4(input.tex.x, 0.0f, 0, 1.0f);
	output.Diffuse = DiffuseMap.Sample(AniSam, input.tex);
	output.Normal = float4(input.tbn[2], 0.0f);

	//input.tbn[0] = normalize(input.tbn[0]);
	//input.tbn[1] = normalize(input.tbn[1]);
	//input.tbn[2] = normalize(input.tbn[2]);
	//float3 normal = NormalMap.Sample(AniSam, input.tex);
	//normal = normal * 2.0f - 1.0f;
	//normal = normalize(mul(normal, input.tbn));
	//normal = (normal + 1.0f) * 0.5f;
	//output.Normal.rgb = normal;
	//output.Normal.a = Metallic.Sample(AniSam, input.tex);
	//output.Diffuse.rgb = DiffuseMap.Sample(AniSam, input.tex);
	//output.Diffuse.a = Roughness.Sample(AniSam, input.tex);
	return output;
}