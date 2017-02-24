cbuffer matrixBuffer: register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4x4 viewInverse;
	float4x4 projectionInverse;
	float4 dt;
	float4 eyePos;
}

cbuffer matrixBuffer: register(b1)
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
	float4 Normal : NORMAL0;
	float2 Tex : TEXCOORD0;
	float4 wPos : POSITION0;
	float4 wNormal : NORMAL1;
};

static float halfAngle = radians(180.0f);
static float quarterAngle = radians(90.0f);

static float4x4 halfYrot = {
	{ cos(halfAngle), 0.0f, sin(halfAngle), 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ -sin(halfAngle), 0.0f, cos(halfAngle), 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }
};

static float4x4 quarterYrot = {
	{ cos(quarterAngle), 0.0f, sin(quarterAngle), 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ -sin(quarterAngle), 0.0f, cos(quarterAngle), 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }
};

[maxvertexcount(24)]
void GS_main(triangle VS_OUT input[3] : SV_POSITION, inout TriangleStream< GS_OUT > output)
{
	GS_OUT element;

	for (uint i = 0; i < 3; i++)
	{
		element.Normal = normalize(input[i].Normal);
		element.Pos = input[i].Pos + (element.Normal * 0.5f);
		element.wPos = mul(element.Pos, world);
		element.Pos = mul(element.Pos, mul(world, mul(view, projection)));
		element.Tex = input[i].Tex;
		element.wNormal = mul(element.Normal, world);
		output.Append(element);
	}

	output.RestartStrip();

	for (i = 0; i < 3; i++)
	{
		element.Normal = normalize(mul(input[i].Normal, halfYrot));
		element.Pos = mul(input[i].Pos, halfYrot) + (element.Normal * 0.5f);
		element.wPos = mul(element.Pos, world);
		element.Pos = mul(element.Pos, mul(world, mul(view, projection)));
		element.Tex = input[i].Tex;
		element.wNormal = mul(element.Normal, world);
		output.Append(element);
	}

	output.RestartStrip();

	for (i = 0; i < 3; i++)
	{
		element.Normal = normalize(mul(input[i].Normal, quarterYrot)) * -1.0f;
		element.Pos = mul(input[i].Pos, transpose(quarterYrot)) + (element.Normal * 0.5f);
		element.wPos = mul(element.Pos, world);
		element.Pos = mul(element.Pos, mul(world, mul(view, projection)));
		element.Tex = input[i].Tex;
		element.wNormal = mul(element.Normal, world);
		output.Append(element);
	}

	output.RestartStrip();

	for (i = 0; i < 3; i++)
	{
		element.Normal = mul(input[i].Normal, quarterYrot);
		element.Pos = mul(input[i].Pos, quarterYrot) + (element.Normal * 0.5f);
		element.wPos = mul(element.Pos, world);
		element.Pos = mul(element.Pos, mul(world, mul(view, projection)));
		element.Tex = input[i].Tex;
		element.wNormal = mul(element.Normal, world);
		output.Append(element);
	}
}