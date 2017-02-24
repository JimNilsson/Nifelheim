struct VS_OUT
{
	float3 wPos : POSITION;
	float3 color : COLOR;
	float3 lightOrigin : ORIGIN;
	float range : RANGE;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float3 uniformDirection : NORMAL0;
	float3 color : NORMAL1;
	float innerDepth : SCALAR;
};

cbuffer PerFrameBuffer : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gViewProj; //This is the only one we need in this shader
	float4x4 gInvView;
	float4x4 gInvViewProj;
	float4 gCamPos; //The w-component is just padding
}


float RayVsPlane(float3 o, float3 d, float3 planeNormal, float planeD)
{
	return 0.0f;
}

[maxvertexcount(12)]
void main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> output)
{
	
	
	float3 v1 = normalize(input[0].wPos - input[0].lightOrigin);
	float3 v2 = normalize(input[0].wPos - input[0].lightOrigin);
	float3 v3 = normalize(input[0].wPos - input[0].lightOrigin);

	float3 e1 = input[0].wPos - input[1].wPos;
	float3 e2 = input[0].wPos - input[2].wPos;

	GS_OUT element;
	element.uniformDirection = normalize(cross(e1, e2));

	float3 base1 = input[0].lightOrigin + v1 * input[0].range;
	float3 base2 = input[0].lightOrigin + v2 * input[0].range;
	float3 base3 = input[0].lightOrigin + v3 * input[0].range;

	float3 p1 = (base1 + base2) / 2;
	float3 p2 = (base1 + base3) / 2;
	float3 p3 = (base2 + base3) / 2;

	float3 t1 = normalize(base1 - p1);
	float3 t2 = normalize(base2 - p2);
	float3 t3 = normalize(base3 - p3);

	float3 to1 = normalize(input[0].lightOrigin - p1);
	float3 to2 = normalize(input[0].lightOrigin - p2);
	float3 to3 = normalize(input[0].lightOrigin - p3);

	float d1 = normalize(cross(to1, t1));
	float d2 = normalize(cross(to1, t1));
	float d3 = normalize(cross(to1, t1));

	float3 tpn1 = normalize(cross(v1, v2));
	float3 tpn2 = normalize(cross(v3, v1));
	float3 tpn3 = normalize(cross(v2, v3));

	float dp1 = dot(tpn1, input[0].lightOrigin);
	float dp2 = dot(tpn2, input[0].lightOrigin);
	float dp3 = dot(tpn3, input[0].lightOrigin);

	float shortest1 = min(RayVsPlane(p1, d1, tpn2, dp2), RayVsPlane(p1, d1, tpn3, dp3));
	float shortest2 = min(RayVsPlane(p2, d2, tpn1, dp1), RayVsPlane(p2, d2, tpn3, dp3));
	float shortest3 = min(RayVsPlane(p3, d3, tpn1, dp1), RayVsPlane(p3, d3, tpn2, dp2));

	float bd1 = RayVsPlane(normalize(p3 - base1), base1, tpn3, dp3);
	float bd2 = RayVsPlane(normalize(p2 - base2), base2, tpn3, dp3);
	float bd3 = RayVsPlane(normalize(p1 - base3), base3, tpn3, dp3);

	//First triangle
	//"tip" of pyramid
	element.pos = mul(float4(input[0].lightOrigin, 1.0f), gViewProj);
	element.color = input[0].color;
	element.innerDepth = 0.0f;
	output.Append(element);
	//Base1
	element.pos = mul(float4(base1, 1.0f), gViewProj);
	element.color = input[1].color;
	element.innerDepth = bd1;
	output.Append(element);
	//Middlebase1
	element.pos = mul(float4(p1, 1.0f), gViewProj);
	element.color = (input[1].color + input[0].color) / 2.0f;
	element.innerDepth = shortest1;
	output.Append(element);
	output.RestartStrip();


}