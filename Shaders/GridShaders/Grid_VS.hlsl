cbuffer matrixBuffer : register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4x4 viewInverse;
	float4x4 projectionInverse;
	float4 dt;
	float4 eyePos;
}

cbuffer matrixBuffer : register(b1)
{
	float4x4 world;
}

struct VS_IN
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), mul(world, mul(view, projection)));
	output.Normal = float4(input.Normal, 0.0f);
	output.Tex = input.Tex;

	return output;
}