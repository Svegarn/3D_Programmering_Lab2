cbuffer matrixBuffer: register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4 dt;
	float4 eyePos;
}

cbuffer matrixBuffer: register(b1)
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
	float4 Normal : NORMAL0;
	float2 Tex : TEXCOORD0;
	float4 wPos : POSITION0;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), mul(world, mul(view, projection)));
	output.Normal = mul(float4(input.Normal, 0.0f), world);
	output.Tex = input.Tex;
	output.wPos = mul(float4(input.Pos, 1.0f), world);

	return output;
}