#include "Header.hlsli"

Texture2D<float4>		Texture2DMaterial	: register(t0);
SamplerState			SamplerLinear		: register(s0);

struct PS_INPUT
{
	float4 vProjPos					: SV_POSITION;
	float3 vWorldNormal				: TEXCOORD0;
	float2 vTexCoord				: TEXCOORD1;
};

void VS_Render(in VS_INPUT_INSTANCED input, out PS_INPUT output)
{
	float4 vInstPosition = mul(float4(input.vPosition, 1.0f), input.mInstance);

	output.vProjPos = mul(vInstPosition, g_mViewProj);
	output.vWorldNormal = normalize(mul(input.vNormal, (float3x3)input.mInstance));
	output.vTexCoord = input.vTexCoord;
}

float4 PS_Render(PS_INPUT input) : SV_TARGET
{
	float4 vColor = float4(0.2f, 0.8f, 0.0f, 1.0f);

	if (g_bIsTexture)
		vColor = Texture2DMaterial.Sample(SamplerLinear, input.vTexCoord);
	else
		vColor.xyz = g_Material.vDiffuse;

	if (vColor.a <= 0.1f)
		discard;

	//vColor.xyz -= 0.0f;

	//vColor = float4(input.vTexCoord, 0.0f, 1.0f);

	vColor.xyz *= max(0.7f, dot(-float3(0.5f, -0.5f, -0.5f), input.vWorldNormal));

	return vColor;
}