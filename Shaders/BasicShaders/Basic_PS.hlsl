Texture2D objectTexture : register(t0);
SamplerState textureSampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4x4 viewInverse;
	float4x4 projectionInverse;
	float4 dt;
	float4 eyePos;
}

cbuffer MaterialData : register(b1) {
	float4 color; // R, G, B, A
	float4 specular; // R, G, B, Shinyness
	float4 misc; // uvAnimationSpeed, Padding, Padding, Padding
	int4 hasTexture;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL0;
	float2 Tex : TEXCOORD0;
	float4 wPos : POSITION0;
};

struct PS_OUT
{
	float4 AlbedoTex: SV_Target0;
	float4 NormalTex: SV_Target1;
	float4 WposTex: SV_Target2;
	float4 MaterialTex: SV_Target3;
};

PS_OUT PS_main(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	float2 animTex = input.Tex;
	animTex.x += dt.y * misc.x;

	// Pulsing
	//outColor.r *= (0.5f * sin(dt.y * 0.002)) + 0.5f;

	// Gbuffers
	if (hasTexture.x == 1)
		output.AlbedoTex = objectTexture.Sample(textureSampler, animTex);
	else
		output.AlbedoTex = color;

	output.NormalTex = float4(normalize(input.Normal).xyz, 0.0f);
	output.WposTex = input.wPos;
	output.MaterialTex = float4(0.0f, 0.0f, 0.0f, specular.w);

	return output;
};