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

float3 Fr_Schlick(float3 F0, float F90, float cosIncident)
{
	return F0 + (F90 - F0) * pow(1.0f - cosIncident, 5.0f);
}
float Vis_SmithGGXHeightCorrelated(float NdL, float NdV, float alpha)
{

	float alphaSq = alpha * alpha;
	float lambdaV = NdL * sqrt((-NdV * alphaSq + NdV) * NdV + alphaSq);
	float lambdaL = NdV * sqrt((-NdL * alphaSq + NdL) * NdL + alphaSq);
	return 0.5f / (lambdaV + lambdaL + 1e-5f);
}
float NDF_GGX(float NdH, float m)
{
	float mSq = m * m;
	float x = (NdH * mSq - NdH) * NdH + 1.0f;
	return mSq / (3.141592f * x * x + 1e-5f);
}

float4 main(VS_OUT input) : SV_TARGET
{
	float depth = Depth.Sample(AniSam, input.tex).r;
	float x = input.tex.x * 2.0f - 1.0f;
	float y = (1.0f - input.tex.y) * 2.0f - 1.0f;
	float4 projPos = float4(x, y, depth, 1.0f);
	
	float4 posVS = mul(projPos, gInvProj);
	posVS.xyz /= posVS.w;
	//return posVS;
	//return float4(posVS.z, posVS.z, posVS.z, 1.0f);
	float3 normal = Normal.Sample(AniSam, input.tex).rgb;
	//return float4(normal, 1.0f);
	normal = normalize((normal * 2.0f) - 1.0f);
//	normal.z *= -1.0f;
	//normal = mul(float4(normal, 0.0f), gInvView);
	//return float4(normal, 1.0f);
	float3 diffuse = Diffuse.Sample(AniSam, input.tex).rgb;
	
	float3 pos = posVS.xyz;
	//float3 col = float3(0.0f, 0.0f, 0.0f);
	float3 diff = float3(0.0f, 0.0f, 0.0f);
	float3 spec = float3(0.0f, 0.0f, 0.0f);
	for (uint i = 0; i < pointLightCount; ++i)
	{
		float3 lightPos = mul(float4(pointLights[i].position.xyz, 1.0f), gView).xyz;
		//float3 lightPos = pointLights[i].position;
		float3 surfaceToLight = lightPos - pos;
		float distance = length(surfaceToLight);
		if (distance < pointLights[i].range)
		{
			surfaceToLight /= distance;
			float lightAmount = dot(normal, surfaceToLight);
			//return float4(lightAmount, lightAmount, lightAmount, 1.0f);
			if (lightAmount > 0.0f)
			{
			//	diffuse.rgb += lightAmount * pointLights[i].color / distance;
				/*float3 v = normalize(-posVS.xyz);
				float3 h = normalize(surfaceToLight + v);
				float NdL = saturate(dot(normal, surfaceToLight));
				float NdH = saturate(dot(normal, h));
				float NdV = abs(dot(normal, v));
				float LdH = saturate(dot(surfaceToLight, h));
				float roughness = 0.5f;
				float linRoughness = roughness * roughness;
				float3 albedo = diffuse;

				float F0 = lerp(0.04f, diffuse, 0.1f);
				float3 F = Fr_Schlick(F0, 1.0f, LdH);
				float Vis = Vis_SmithGGXHeightCorrelated(NdL, NdV, roughness);
				float D = NDF_GGX(NdH, roughness);
				float3 spec = F * Vis * D;

				float A = 1.0f - 0.5f * linRoughness / (linRoughness + 0.57f);
				float B = 0.45f * linRoughness / (linRoughness + 0.09f);
				float LdV = saturate(dot(surfaceToLight, v));
				col += pointLights[i].color * pointLights[i].intensity * saturate(1.0f-distance / pointLights[i].range) * (albedo / 3.141592f) * (A + B * saturate(LdV - NdV * NdL) / max(NdL, NdV));*/
				float attenuation = 1.0f / ((1.0f + distance * 0.7412f) + distance * distance * 0.05f);
				diff += lightAmount * pointLights[i].color * attenuation * pointLights[i].intensity;
				//float3 ref = normalize(2.0f * lightAmount * normal - surfaceToLight);
				//float specf = pow(saturate(dot(ref, -posVS.xyz)), 2000.0f);
				//spec += pointLights[i].color * specf * attenuation;

				float3 H = normalize(surfaceToLight - normalize(posVS.xyz));
				float NdH = saturate(dot(normal, H));
				spec += pointLights[i].color * pow(NdH, 12.0f) * pointLights[i].intensity * attenuation;


			}
		}
	}
	return float4(saturate(diffuse * diff + diffuse * spec + diffuse * 0.2f), 1.0f);
}