struct VS_OUT
{
	float4 posH : SV_POSITION;
	float4 posVS : POSITION;
	float3 toEye : NORMAL;
	float2 tex : TEXCOORD;
	float3x3 tbn : TBNMATRIX;
};

[maxvertexcount(3)]
void main(triangle VS_OUT input[3],	inout TriangleStream<VS_OUT> output)
{
	//float3 v1 = input[0].posVS.xyz - input[1].posVS.xyz;
	//float3 v2 = input[0].posVS.xyz - input[2].posVS.xyz;
	//float3 v3 = input[1].posVS.xyz - input[2].posVS.xyz;
	//float c0 = abs(dot(v1, v2));
	//float c2 = abs(dot(v2, v3));

	//if (c0 < c2)
	//{
	//	//input[0].tex = float2(0.0f, 0.0f);
	//	input[1].tex = float2(-length(v2), 1.0f);
	//	input[2].tex = float2(1.0f, -length(v1));
	//	input[0].tex = float2(-length(v2), -length(v1));
	//	/*input[1].tex.x = 0.0f;
	//	input[1].tex.y = 1.0f + length(v1);*/
	//	/*input[2].tex.y = 0.0f;
	//	input[2].tex.x = 1.0f + length(v2);*/
	//}
	//else
	//{
	//	input[2].tex = float2(length(v3),length(v2));
	//	input[1].tex = float2(-1.0f, length(v2));
	//	input[0].tex = float2(length(v3), -1.0f);
	//}

	for (uint i = 0; i < 3; i++)
	{
		VS_OUT element;
		element = input[i];
		output.Append(element);
	}
	

	



}