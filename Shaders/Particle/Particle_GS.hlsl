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
	float4 Pos : POSITION;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float4 wPos : POSITION;
};

[maxvertexcount(4)]
void GS_main(point VS_OUT input[1] : SV_POSITION, inout TriangleStream< GS_OUT > output)
{
	GS_OUT element;

	float3 upVec = float3(0.0f, 1.0f, 0.0f);
	float3 lookAt = normalize(eyePos.xyz - input[0].Pos.xyz);
	float3 rightVec = normalize(cross(lookAt.xyz, upVec));

	upVec = normalize(cross(lookAt.xyz, rightVec.xyz));
	rightVec = normalize(cross(lookAt, upVec));

	float4 v1 = float4(input[0].Pos.xyz + upVec + rightVec, 1.0f);
	float4 v2 = float4(input[0].Pos.xyz + upVec - rightVec, 1.0f);
	float4 v3 = float4(input[0].Pos.xyz - upVec + rightVec, 1.0f);
	float4 v4 = float4(input[0].Pos.xyz - upVec - rightVec, 1.0f);


	element.Pos = mul(v1, mul(view, projection));
	element.wPos = float4(abs(upVec), 1.0f);
	output.Append(element);

	element.Pos = mul(v2, mul(view, projection));
	element.wPos = float4(abs(upVec), 1.0f);
	output.Append(element);

	element.Pos = mul(v3, mul(view, projection));
	element.wPos = float4(abs(upVec), 1.0f);
	output.Append(element);

	element.Pos = mul(v4, mul(view, projection));
	element.wPos = float4(abs(upVec), 1.0f);
	output.Append(element);
}