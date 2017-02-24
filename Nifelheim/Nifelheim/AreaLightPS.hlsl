struct PS_OUT
{
	float4 AreaLightContribution : SV_TARGET0;
};

Texture2D Depth : register(t0);

float4 main(GS_OUT input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}