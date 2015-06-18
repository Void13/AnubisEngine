struct CBMaterial
{
	float3			vDiffuse;
	float			fOpacity;

	float3			vSpecular;
	float			fShininess;

	float3			vAmbient;
	float			fShininessStrength;

	float3			vEmissive;
	float			fRefraction;

	float3			vTransparent;
	int				bIsWireframe;
};

cbuffer CBPerFrame : register(b0)
{
	matrix			g_mViewProj;

	CBMaterial		g_Material;

	float			g_fFarPlane;
	int				g_bIsTexture;
	float			g_Unused[2];
}

struct VS_INPUT
{
	float3 vPosition				: POSITION;
	float3 vNormal					: NORMAL;
	float2 vTexCoord				: TEXCOORD0;
};

struct VS_INPUT_INSTANCED
{
	float3 vPosition				: POSITION;
	float3 vNormal					: NORMAL;
	float2 vTexCoord				: TEXCOORD0;

	matrix mInstance				: INSTANCE;
};

struct VS_ANIMATED_INPUT_INSTANCED
{
	float3 vPosition				: POSITION;
	float3 vNormal					: NORMAL;
	float2 vTexCoord				: TEXCOORD0;
	float4 vWeights					: BONE_WEIGHTS;
	float4 vIndices					: BONE_INDICES;

	matrix mInstance				: INSTANCE;
};