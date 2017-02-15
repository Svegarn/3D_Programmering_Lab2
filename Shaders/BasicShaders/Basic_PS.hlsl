Texture2D bthTexture: register(t0);
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

	input.Tex.x += (dt.y * 0.0005);
	float3 lightDir = float3(0.0f, -1.0f, 0.0f);
	float4 outColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 normal = normalize(input.Normal.xyz);

	// Diffuse
	float3 materialColor = bthTexture.Sample(textureSampler, input.Tex).xyz;
	//materialColor.y *= 0.5f * sin(dt.y * sin(input.Tex.x/input.Tex.y) * 0.02) + 0.5f;
	float diffuseIntensity = max(dot(normal, -lightDir), 0.0f);
	float3 diffuseLight = materialColor * diffuseIntensity;

	// Specular
	float specularIntensity = 3.0f;
	float3 pixelToEye = normalize(eyePos - input.wPos).xyz;
	float3 reflection = reflect(lightDir, normal);
	float3 specularLight = pow(max(dot(reflection, pixelToEye), 0.0f), specularIntensity);


	// Pulsing
	//outColor.x *= (0.5f * sin(dt.y * 0.002)) + 0.5f;

	outColor = float4(diffuseLight + specularLight, 1.0f);

	//return float4 (input.wPos.xyz, 1.0f);

	output.AlbedoTex = outColor;
	output.NormalTex = float4(normal, 0.0f);
	output.MaterialTex = input.wPos;

	return output;
};