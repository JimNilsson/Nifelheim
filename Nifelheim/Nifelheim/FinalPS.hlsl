struct PointLight
{
	float3 position;
	float range;
	float3 color;
	float intensity;
};

struct DirLight
{
	float3 direction;
	float intensity;
	float3 color;
	float pad;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

cbuffer LightBuffer : register(b0)
{
	PointLight pointLights[64];
	DirLight dirLights[8];
	uint pointLightCount;
	uint dirLightCount;
	float pad;
	float pad2;
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
TextureCube Sky : register(t3);

float fresnel(float f0, float3 n, float3 l)
{
	return f0 + (1.0f - f0) * pow(1.0f - dot(n, l), 5.0f);
}

float distribution(float3 n, float3 h, float roughness)
{
	float msq = roughness * roughness;
	float NdHsq = max(dot(n, h), 0.0f);
	NdHsq = NdHsq * NdHsq;
	return exp((NdHsq - 1.0f) / (msq * NdHsq)) / (3.14159265f * msq * NdHsq * NdHsq);
}

float geometry(float3 n, float3 h, float3 v, float3 l, float roughness)
{
	float NdH = dot(n, h);
	float NdL = dot(n, l);
	float NdV = dot(n, v);
	float VdH = dot(v, h);
	float cNdL = max(NdL, 0.0f);
	float cNdV = max(NdV, 0.0f);
	return min(min(2.0f * NdH * cNdV / VdH, 2.0f * NdH * cNdL / VdH), 1.0f);
}

float diffEnergy(float f0, float3 n, float3 l)
{
	return 1.0f - fresnel(f0, n, l);
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
	float4 normalSam = Normal.Sample(AniSam, input.tex);
	float3 normal = normalize((normalSam.xyz * 2.0f) - 1.0f);
	float metallic = normalSam.w;
	float4 diffuse = Diffuse.Sample(AniSam, input.tex);
	float roughness = diffuse.a;
	if (length(normalSam.xyz) < 0.1f)
		return float4(diffuse.xyz, 1.0f);

	
	//return float4(diffuse, 1.0f);
	float3 pos = posVS.xyz;
	//float3 col = float3(0.0f, 0.0f, 0.0f);
	float3 plContribution = float3(0.0f, 0.0f, 0.0f);;
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
			if (lightAmount > 0.0f)
			{
			
				float attenuation = 1.0f / ((1.0f + distance * 0.7412f) + distance * distance * 0.05f);
				diff += lightAmount * pointLights[i].color * attenuation * pointLights[i].intensity * roughness;
				float3 H = normalize(surfaceToLight - normalize(posVS.xyz));
				float NdH = saturate(dot(normal, H));
				spec += pointLights[i].color * pow(NdH, 12.0f) * pointLights[i].intensity * attenuation * (1.0f - roughness);


			}
		}

	}

	//for (uint i = 0; i < dirLightCount; ++i)
	//{
	//	float lightAmount = dot(dirLights[i].direction, normal);
	//	if (lightAmount > 0.0f)
	//	{
	//		diff += lightAmount * dirLights[i].color * dirLights[i].intensity * roughness;
	//		float3 H = normalize(dirLights[i].direction - normalize(posVS.xyz));
	//		float NdH = saturate(dot(normal, H));
	//		spec += dirLights[i].color * pow(NdH, 12.0f) * pointLights[i].intensity * (1.0f - roughness);
	//	}
	//}


	//float3 viewDir = normalize(posVS);
	//float3 ref = viewDir + (2.0f * normal * dot(normal, -viewDir));
	//ref = mul(float4(ref, 0.0f), gInvView).xyz;
	//float3 refcol = Sky.Sample(AniSam, ref).xyz;
	//float NdS = dot(normal, dir);
	//if(NdS > 0.0f)
	//	diffuse += NdS * refcol * 0.05f + NdS * diffuse * float3(1.0f,0.62f,0.0f) * 0.9f;
	//diffuse += 0.05f * refcol;
	//return saturate(float4(diffuse, 1.0f));
	return float4(saturate(diffuse * diff + diffuse * spec + diffuse * 0.2f), 1.0f);
}