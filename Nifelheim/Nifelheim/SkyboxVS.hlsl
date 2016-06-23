
cbuffer SkyBuffer : register(b0)
{
	float4 gCampos;
	float4x4 gVP;
}

void main(uint VertexID : SV_VertexID, out float4 oPosH : SV_POSITION, out float3 oTex : TEXCOORD)
{
	float3 positions[14] = {
		float3(-1.0f, -1.0f, -1.0f),
		float3(1.0f, -1.0f, -1.0f),
		float3(1.0f, -1.0f,  1.0f),
		float3(1.0f,  1.0f, -1.0f),
		float3(1.0f,  1.0f,  1.0f),
		float3(-1.0f,  1.0f,  1.0f),
		float3(1.0f, -1.0f,  1.0f),
		float3(-1.0f, -1.0f,  1.0f),
		float3(-1.0f, -1.0f, -1.0f),
		float3(-1.0f,  1.0f,  1.0f),
		float3(-1.0f,  1.0f, -1.0f),
		float3(1.0f,  1.0f, -1.0f),
		float3(-1.0f, -1.0f, -1.0f),
		float3(1.0f, -1.0f, -1.0f)
	};
	oPosH = mul(float4(positions[VertexID] + gCampos.xyz, 1.0f), gVP).xyww;
	
	oTex = positions[VertexID];
}