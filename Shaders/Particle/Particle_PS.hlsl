struct GS_OUT
{
	float4 Pos : SV_POSITION; // Vertex ID
	float4 wPos : POSITION;
};

float4 PS_main(GS_OUT input) : SV_Target
{
	return float4(input.wPos.xyz, 1.0f);
}