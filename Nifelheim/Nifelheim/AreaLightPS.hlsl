struct PS_OUT
{
	float4 AreaLightContribution : SV_TARGET0;
};
struct GS_OUT
{
	float4 pos : SV_POSITION;
	float3 uniformDirection : NORMAL0;
	float3 color : NORMAL1;
	float innerDepth : SCALAR;
};
Texture2D Depth : register(t0);

float4 main(GS_OUT input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}