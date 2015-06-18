#include "Header.hlsli"

Texture2D<float4>		Texture2DMaterial	: register(t0);
SamplerState			SamplerLinear		: register(s0);

struct PS_INPUT
{
	float4 vProjPos					: SV_POSITION;
	float3 vWorldNormal				: TEXCOORD0;
	float2 vTexCoord				: TEXCOORD1;
	float3 vEyeToPixel				: TEXCOORD2;
};

void VS_Render(in VS_INPUT_INSTANCED input, out PS_INPUT output)
{
	float4 vInstPosition = mul(float4(input.vPosition, 1.0f), input.mInstance);
	//float4 vInstPosition = float4(input.vPosition, 1.0f);

	output.vProjPos = mul(vInstPosition, g_mWVP);
	output.vWorldNormal = normalize(mul(input.vNormal, (float3x3)input.mInstance));
	output.vTexCoord = input.vTexCoord;
	output.vEyeToPixel = g_vEye - vInstPosition.xyz;
}

static float fExposure = -2.0f;

float3 HDR(float3 vLDR)
{
	return 1.0f - exp(fExposure * vLDR);
}

float3 ToneMap(float3 vHDR)
{
	return (vHDR / (vHDR + 1.0f));
}

float GetMiePhase(float fCos, float fCos2)
{
	return g_vHG.x * (1.0f + fCos2) / pow(g_vHG.y - g_vHG.z * fCos, 1.5f);
}

float GetRayleighPhase(float fCos2)
{
	//return 0.75f + 0.75f * fCos2;
	return 0.75f + (0.9f * fCos2);
}

float ComputeVolumetricFog(float3 vCameraToWorldPos)
{
	static float fC = 0.002f; // height falloff
	static float fB = 0.001f; // global density

	float f = fB * exp(-fC * g_vEye.y);

	float fFogInt = exp(-(f * length(vCameraToWorldPos)));
	//float fFogInt = f * length(vCameraToWorldPos);

	static const float fSlopeThreshold = 0.01f;
	if (abs(vCameraToWorldPos.y) > fSlopeThreshold)
	{
		float t = fC * vCameraToWorldPos.y;

		fFogInt *= (1.0f - exp(-t)) / t;
	}

	return fFogInt;
	//return exp(-fFogInt);
}

#define K_MATCH_FLAME		1750
#define K_CANDLE_FLAME		1880
#define K_SUN_SUNRISE		2500
#define K_SUN_SUNSET		2500
#define K_TUNGSTEN_BULB		2700
#define K_QUARTZ_LIGHT		3350
#define K_SUN_NOON			5200
#define K_DAYLIGHT			6000
#define K_SUN_CLOUDY_HAZY	6000
#define K_SKY_OVERCAST		6750
#define K_OUTDOOR_SHADE		7500
#define K_SKY_PARTLY_CLOUDY	9000

// available to kelvin from 1000 to 40000
float3 GetColorByTemperature(float fKelvin)
{
	float3 vColor = 0.0f;

	float fTemp = fKelvin * 0.01f;

	if (fTemp <= 66.0f)
	{
		vColor.r = 255.0f;
		vColor.g = 99.4708f * log(fTemp) - 161.1195f;
	}
	else
	{
		vColor.r = 329.6987f * pow(fTemp - 60.0f, -0.1332f);
		vColor.g = 288.1221f * pow(fTemp - 60.0f, -0.0755f);
	}

	if (fTemp >= 66.0f)
	{
		vColor.b = 255.0f;
	}
	else
	{
		if (fTemp <= 19.0f)
		{
			vColor.b = 0.0f;
		}
		else
		{
			vColor.b = 138.5177f * log(fTemp - 10.0f) - 305.0447f;
		}
	}

	vColor = clamp(vColor, 0.0f, 255.0f);
	vColor *= 0.003921f;

	return vColor;
}

float4 PS_Render(PS_INPUT input) : SV_TARGET
{
	//return float4(input.vWorldNormal, 1.0f);

	float4 vColor = float4(0.2f, 0.8f, 0.0f, 1.0f);

	if (g_bIsTexture)
		vColor = Texture2DMaterial.Sample(SamplerLinear, input.vTexCoord);
	else
		vColor = float4(g_vDiffuse, 1.0f);

	if (vColor.a <= 0.1f)
		discard;

	vColor *= max(0.3f, dot(-g_vSunDir, input.vWorldNormal));

	float4 vSkyColor = 0.0f;

	float fCos = dot(g_vSunDir, input.vEyeToPixel) / length(input.vEyeToPixel);
	float fCos2 = fCos * fCos;

	//vSkyColor.a = log2(length(vMie) * 2500.0f);
	vSkyColor.rgb += GetRayleighPhase(fCos2) * g_vGroundC0;

	if (g_vSunDir.y > 0.05f)
	{
		vSkyColor.rgb += GetMiePhase(fCos, fCos2) * g_vGroundC1;

		//vSkyColor.rgb += 0.1f;
		vSkyColor.rgb = HDR(vSkyColor.rgb);
		vSkyColor.rgb += 0.1f;
	}
	else
	{
		vSkyColor.rgb += 0.1f;
		vSkyColor.b += 0.05f;
	}

	vColor.rgb *= vSkyColor.rgb;

	//vColor.rgb += 0.1f;

	vColor.rgb = lerp(vSkyColor.rgb, vColor.xyz, ComputeVolumetricFog(input.vEyeToPixel));

	vColor.rgb *= GetColorByTemperature(K_QUARTZ_LIGHT);

	vColor.rgb = HDR(vColor.rgb);

	return vColor;
}

struct PS_INPUT_RENDER_QUAD
{
	float4 vProjPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

void VS_RENDER_QUAD(in uint vid : SV_VertexID, out PS_INPUT_RENDER_QUAD output)
{
	output.vTexCoord = float2((vid << 1) & 2, vid & 2);
	output.vProjPos = float4((output.vTexCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f)), 0.0f, 1.0f);
}

float4 PS_RENDER_QUAD(PS_INPUT_RENDER_QUAD input) : SV_TARGET
{
	//return float4(input.vProjPos.xy * 0.001f, 0.0f, 1.0f);
	//return float4(input.vTexCoord.xy, 0.0f, 1.0f);

	
	input.vProjPos.xy *= 0.001f;
	input.vProjPos.xy -= float2(0.5f, 0.5f);
	if (input.vProjPos.x * input.vProjPos.x + input.vProjPos.y * input.vProjPos.y > 0.25f)
	{
		discard;
		return float4(0.8f, 0.8f, 0.0f, 1.0f);
	}

	//return float4(DepthBuffer.Sample(SamplerLinear, float3(input.vTexCoord, SPLIT)).r, 0.0f, 0.0f, 1.0f);
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}