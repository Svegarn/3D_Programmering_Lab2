Texture2D shadowTex : register(t0);
Texture2D albedoTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D wposTex : register(t3);
Texture2D materialTex : register(t4);
Texture2D gridTex : register(t5);

SamplerState textureSampler : register(s0);

cbuffer matrixBuffer : register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4x4 viewInverse;
	float4x4 projectionInverse;
	float4 dt;
	float4 eyePos;
}

struct VS_OUT
{
	float4 Pos : SV_POSITION; // Vertex ID
	float2 Tex : TEXCOORD;
};

float3 WorldPosFromDepth(float depth, float2 texCoord);
float map(float s, float a1, float a2, float b1, float b2);

float4 PS_main(VS_OUT input) : SV_Target
{
	float4 outColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Diffuse //
	float3 lightDir = float3(0.0f, -1.0f, 0.0f);
	float3 normal = normalize(normalTex.Sample(textureSampler, input.Tex).xyz);
	float3 materialColor = albedoTex.Sample(textureSampler, input.Tex).xyz;
	float diffuseIntensity = max(dot(normal, -lightDir), 0.0f);
	outColor.xyz = materialColor * diffuseIntensity;

	// Specular //
	float shinyness = materialTex.Sample(textureSampler, input.Tex).w;
	if (shinyness > 0.000001f)
	{
		float3 pixelToEye = normalize(eyePos.xyz - wposTex.Sample(textureSampler, input.Tex).xyz);
		float3 reflection = reflect(lightDir, normal);
		float3 specularLight = pow(max(dot(reflection, pixelToEye), 0.0f), shinyness);

		outColor = float4(outColor.xyz + specularLight, 1.0f);
	}

	// Temp crosshair
	float tempX = (0.25f - input.Tex.x) + 0.25f;
	float tempY = (0.25f - input.Tex.y) + 0.25f;

	float rad = (tempX * tempX) + (tempY * tempY) * 0.35f;

	if (rad < 0.000005f)
		outColor = float4(0.9f, 0.0f, 0.0f, 1.0f);

	float4 grid = gridTex.Sample(textureSampler, input.Tex);
	if (grid.g > 0.01f)
		outColor = grid * grid.a;


	// Blits
	if (input.Tex.x < 0.5f && input.Tex.y < 0.5f)
		outColor = shadowTex.Sample(textureSampler, input.Tex);
	else if (input.Tex.x > 0.5f && input.Tex.y < 0.5f)
		outColor = albedoTex.Sample(textureSampler, input.Tex);
	else if (input.Tex.x < 0.5f && input.Tex.y > 0.5f)
		outColor = float4(normal, 0.0f);


	return outColor;
};

float3 WorldPosFromDepth(float depth, float2 texCoord)
{
	float z = depth * 2.0 - 1.0;

	float4 clipSpacePosition = float4(texCoord * 2.0 - 1.0, z, 1.0);
	float4 viewSpacePosition = mul(projectionInverse, clipSpacePosition);

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	float4 worldSpacePosition = mul(viewInverse, viewSpacePosition);

	return worldSpacePosition.xyz;
}

float map(float s, float a1, float a2, float b1, float b2)
{
	return b1 + (s - a1)*(b2 - b1) / (a2 - a1);
}