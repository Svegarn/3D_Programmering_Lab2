Texture2D bthTexture: register(t0);
SamplerState textureSampler: register(s0);

struct VS_OUT
{
	float4 Pos : SV_POSITION; // Vertex ID
	float2 Tex : TEXCOORD;
};


float4 PS_main(VS_OUT input) : SV_Target
{
	float4 outColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	outColor = bthTexture.Sample(textureSampler, input.Tex);

	return outColor;
};