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

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
};

[maxvertexcount(2)]
void GS_main(line VS_OUT input[2] : SV_POSITION, inout LineStream< GS_OUT > output)
{
	GS_OUT element;

	float3 axis = cross(normalize(input[0].Pos - input[1].Pos).xyz, normalize(input[0].Normal).xyz)  * 0.0005f;

	element.Pos = mul(input[0].Pos, mul(world, mul(view, projection)));
	output.Append(element);

	element.Pos = mul(input[1].Pos, mul(world, mul(view, projection)));
	output.Append(element);
}