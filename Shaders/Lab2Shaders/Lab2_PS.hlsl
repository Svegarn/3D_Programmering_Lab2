Texture2D bthTexture: register(t0);
SamplerState textureSampler: register(s0);

cbuffer matrixBuffer: register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4 dt;
	float4 eyePos;
}

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL0;
	float2 Tex : TEXCOORD0;
	float4 wPos : POSITION0;
	float4 wNormal : NORMAL1;
};

float4 PS_main(GS_OUT input) : SV_Target
{
	float3 lightDir = float3(0.0f, 0.0f, 1.0f);
	float4 outColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 normal = input.wNormal.xyz;

	// Diffuse
	float3 materialColor = bthTexture.Sample(textureSampler, input.Tex).xyz;
	float diffuseIntensity = max(dot(normal, -lightDir), 0.0f);
	float3 diffuseLight = materialColor * diffuseIntensity;

	// Specular
	float specularIntensity = 2000.0f;
	float3 pixelToEye = normalize(eyePos - input.wPos).xyz;
	float3 reflection = reflect(lightDir, normal);
	float3 specularLight = pow(max(dot(reflection, pixelToEye), 0.0f), 3.0f);


	// Pulsing
	//outColor.x *= (0.5f * sin(dt.y * 0.002)) + 0.5f;

	outColor = float4(diffuseLight + specularLight, 1.0f);

	//return float4 (input.wPos.xyz, 1.0f);
	return outColor;
}