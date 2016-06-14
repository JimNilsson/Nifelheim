struct PointLight
{
	float3 position;
	float range;
	float3 color;
	float intensity;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

cbuffer LightBuffer : register(b0)
{
	PointLight pointLights[512];
	uint pointLightCount;
}

cbuffer PerFrameBuffer : register(b1)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gViewProj;
	float4x4 gInvView;
	float4x4 gInvViewProj;
	float4x4 gInvProj;
	float4 gCamPos; //The w-component is just padding
}

SamplerState AniSam : register(s0);

Texture2D Diffuse : register(t0);
Texture2D Normal : register(t1);
Texture2D Depth : register(t2);

float4 main(VS_OUT input) : SV_TARGET
{
	float depth = Depth.Sample(AniSam, input.tex).r;
	float x = input.tex.x * 2.0f - 1.0f;
	float y = (1.0f - input.tex.y) * 2.0f - 1.0f;
	float4 projPos = float4(x, y, depth, 1.0f);

	float4 posVS = mul(projPos, gInvProj);
	posVS.xyz /= posVS.w;

	float3 normal = Normal.Sample(AniSam, input.tex).rgb;
	normal = (normal - 0.5f) * 2.0f;
	normal.z *= -1.0f;
	
	float3 diffuse = Diffuse.Sample(AniSam, input.tex).rgb;
	float3 pos = posVS.xyz;

	for (uint i = 0; i < pointLightCount; ++i)
	{
		float3 lightPos = mul(float4(pointLights[i].position, 1.0f), gView).xyz;
		float3 lightToPixel = pos - lightPos;
		float distance = length(lightToPixel);
		if (distance < pointLights[i].range)
		{
			lightToPixel /= distance;
			diffuse.rgb += dot(normal, lightToPixel) * pointLights[i].color / distance;
		}
	}
	
	return float4(diffuse,1.0f);
}