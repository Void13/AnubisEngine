#include "Header.hlsli"

struct GS_WIRE_INPUT
{
	float4 vWVPPos					: POSITION;
	float3 vWorldNormal				: TEXCOORD0;
	float2 vTexCoord				: TEXCOORD1;
};

struct PS_WIRE_INPUT
{
	float4 vWVPPos					: SV_POSITION;
	noperspective float3 Heights	: TEXCOORD0;
	float fDepth					: TEXCOORD1;

	float3 vWorldNormal				: TEXCOORD2;
	float2 vTexCoord				: TEXCOORD3;
};

struct OM_WIRE_INPUT
{
	float4 vFinalColor				: SV_TARGET;
};

GS_WIRE_INPUT VS_WIRE_Render(in VS_INPUT_INSTANCED input)
{
	GS_WIRE_INPUT output;

	float4 vInstPosition = mul(float4(input.vPosition, 1.0f), input.mInstance);
	output.vWVPPos = mul(vInstPosition, g_mViewProj);
	output.vWorldNormal = normalize(mul(input.vNormal, (float3x3)input.mInstance));
	output.vTexCoord = input.vTexCoord;

	return output;
}

[maxvertexcount(3)]
void GS_WIRE_Render(triangle GS_WIRE_INPUT input[3],
					inout TriangleStream<PS_WIRE_INPUT> outStream )
{
	PS_WIRE_INPUT output;
	
	output.vWVPPos = input[0].vWVPPos;
	output.Heights = float3(0, 0, 1);
	output.fDepth = input[0].vWVPPos.z / g_fFarPlane;
	output.vWorldNormal = input[0].vWorldNormal;
	output.vTexCoord = input[0].vTexCoord;
	outStream.Append( output );

	output.vWVPPos = input[1].vWVPPos;
	output.Heights = float3(0, 1, 0);
	output.fDepth = input[1].vWVPPos.z / g_fFarPlane;
	output.vWorldNormal = input[1].vWorldNormal;
	output.vTexCoord = input[1].vTexCoord;
	outStream.Append( output );

	output.vWVPPos = input[2].vWVPPos;
	output.Heights = float3(1, 0, 0);
	output.fDepth = input[2].vWVPPos.z / g_fFarPlane;
	output.vWorldNormal = input[2].vWorldNormal;
	output.vTexCoord = input[2].vTexCoord;
	outStream.Append( output );

	//outStream.RestartStrip();
}

float evalMinDistanceToEdges(in PS_WIRE_INPUT input)
{
    float dist;

	float3 ddxHeights = ddx( input.Heights );
	float3 ddyHeights = ddy( input.Heights );
	float3 ddHeights2 =  ddxHeights * ddxHeights + ddyHeights*ddyHeights;
	
    float3 pixHeights2 = input.Heights *  input.Heights / ddHeights2 ;
    
    dist = sqrt( min ( min (pixHeights2.x, pixHeights2.y), pixHeights2.z) );
    
    return dist;
}

OM_WIRE_INPUT PS_WIRE_Render(in PS_WIRE_INPUT input)
{
	OM_WIRE_INPUT output;

	output.vFinalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//output.vFinalColor = float4(input.fDepth, 0.0f, 0.0f, 1.0f);
	//return output;

	float lineWidth = 0.0001f;

	lineWidth = lerp(0.0f, lineWidth, 2.0f / pow(2.0f * input.fDepth + 1.0f, 8.0f));

	float dist = evalMinDistanceToEdges(input);

	if (dist > lineWidth * 0.5f + 1.0f)
	{
		//discard;

		output.vFinalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return output;
	}

	dist = clamp((dist - (0.5f * lineWidth - 1.0f)), 0, 2);

	// Alpha is computed from the function exp2(-2(x)^2).
	dist *= dist;
	float alpha = exp2(-2 * dist);

	// Standard wire color
	//output.vFinalColor.xyz = g_vDiffuse;
	output.vFinalColor.xyz = input.vWorldNormal;

	//output.vFinalColor.xyz = float3(0.0f, 1.0f, 0.0f);
	//output.vFinalColor.a = alpha;
	output.vFinalColor.a = 1.0f;

	
	return output;
}