struct VS_OUT
{
	float4 Pos : SV_POSITION; // Vertex ID
	float2 Tex : TEXCOORD;
};

VS_OUT VS_main(uint id: SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	output.Tex = float2((id << 1) & 2, id & 2);
	output.Pos = float4(output.Tex * float2(2, -2) + float2(-1, 1), 0, 1);

	return output;
}