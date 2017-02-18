Texture2D shadowTex: register(t0);
Texture2D albedoTex: register(t1);
Texture2D normalTex: register(t2);
Texture2D materialTex: register(t3);
SamplerState textureSampler: register(s0);

struct VS_OUT
{
	float4 Pos : SV_POSITION; // Vertex ID
	float2 Tex : TEXCOORD;
};


float4 PS_main(VS_OUT input) : SV_Target
{
	float4 outColor = float4(1.0f, 1.0f, 0.0f, 1.0f);

	// Blits
	//if (input.Tex.x < 0.5f && input.Tex.y < 0.5f)
	//	outColor = shadowTex.Sample(textureSampler, input.Tex);
	//else if (input.Tex.x > 0.5f && input.Tex.y < 0.5f)
	//	outColor = albedoTex.Sample(textureSampler, input.Tex);
	//else if (input.Tex.x > 0.5f && input.Tex.y < 0.5f)
	//	outColor = normalTex.Sample(textureSampler, input.Tex);
	//else
	//	outColor = materialTex.Sample(textureSampler, input.Tex);

	// Temp crosshair
	float tempX = (0.25f - input.Tex.x) + 0.25f;
	float tempY = (0.25f - input.Tex.y) + 0.25f;

	float rad = (tempX * tempX) + (tempY * tempY) * 0.35f;

	if (rad < 0.000005f)
		outColor = float4(0.9f, 0.0f, 0.0f, 1.0f);
	else
		outColor = albedoTex.Sample(textureSampler, input.Tex);

	return outColor;
};