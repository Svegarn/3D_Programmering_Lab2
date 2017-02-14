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

	//output.Pos.x = (float)(id / 2) * 4.0f - 1.0f;
	//output.Pos.y = (float)(id % 2) * 4.0f - 1.0f;
	//output.Pos.z = 0.0f;
	//output.Pos.w = 1.0f;

	//output.Tex.x = (float)(id / 2) * 2.0f;
	//output.Tex.y = 1.0f - (float)(id % 2) * 2.0f;

	return output;
}