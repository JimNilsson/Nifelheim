

void main(uint VertexID : SV_VertexID, out float4 oPosH : SV_POSITION, out float2 oTexC : TEXCOORD)
{
	oPosH.x = (VertexID == 2) ? 3.0f : -1.0f;
	oPosH.y = (VertexID == 0) ? -3.0f : 1.0f;
	oPosH.zw = 1.0f;

	oTexC = oPosH.xy * float2(0.5f, -0.5f) + 0.5f;
}