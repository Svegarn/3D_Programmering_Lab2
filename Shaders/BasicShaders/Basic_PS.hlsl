Texture2D objectTexture: register(t0);
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
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL0;
	float2 Tex : TEXCOORD0;
	float4 wPos : POSITION0;
};

struct PS_OUT
{
	float4 AlbedoTex: SV_Target0;
	float4 NormalTex: SV_Target1;
	float4 MaterialTex: SV_Target2;
};

PS_OUT PS_main(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	float2 animTex = input.Tex;
	animTex.x += (dt.y * 0.0005);

	// Pulsing
	//outColor.r *= (0.5f * sin(dt.y * 0.002)) + 0.5f;

	// Gbuffers
	output.AlbedoTex = objectTexture.Sample(textureSampler, animTex);
	output.NormalTex = float4(normalize(input.Normal).xyz, 0.0f);
	output.MaterialTex = input.wPos;

	return output;
};