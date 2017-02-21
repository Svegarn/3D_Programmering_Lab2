Texture2D shadowTex: register(t0);
Texture2D albedoTex: register(t1);
Texture2D normalTex: register(t2);
Texture2D materialTex: register(t3); // materialTex holds wPos atm
Texture2D gridTex: register(t4);

SamplerState textureSampler: register(s0);

cbuffer matrixBuffer: register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4 dt;
	float4 eyePos;
}

struct VS_OUT
{
	float4 Pos : SV_POSITION; // Vertex ID
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	float4 outColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Diffuse //
	float3 lightDir = float3(0.0f, -1.0f, 0.0f);
	float3 normal = normalize(normalTex.Sample(textureSampler, input.Tex).xyz);
	float3 materialColor = albedoTex.Sample(textureSampler, input.Tex).xyz;
	float diffuseIntensity = max(dot(normal, -lightDir), 0.0f);
	float3 diffuseLight = materialColor * diffuseIntensity;

	// Specular //
	float specularIntensity = 20.0f;
	float3 pixelToEye = normalize(eyePos - materialTex.Sample(textureSampler, input.Tex)).xyz; // materialTex holds wPos atm
	float3 reflection = reflect(lightDir, normal);
	float3 specularLight = pow(max(dot(reflection, pixelToEye), 0.0f), specularIntensity);

	outColor = float4(diffuseLight + specularLight, 1.0f);

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

	float4 grid = gridTex.Sample(textureSampler, input.Tex);
	if (grid.g > 0.01f)
		outColor = grid * grid.a;

	return outColor;
};