#include "EnginePCH.h"
#include "Resource_Texture.h"

#pragma comment(lib, "DevIL\\devil.lib")
#pragma comment(lib, "DevIL\\ilu.lib")
#pragma comment(lib, "DevIL\\ilut.lib")

#include "DevIL\\il.h"
#include "DevIL\\ilu.h"
#include "DevIL\\ilut.h"

int g_nNumTextures = 0;

void CTextureResource::VLoadResource(CResourceLoadingData const &LoadingData)
{
	//m_sName = LoadingData.m_sFileName;
	m_bIsInfinite = LoadingData.m_bIsInfinite;

	if (g_nNumTextures == 0)
	{
		ilInit();
		iluInit();
		ilutInit();

		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

		ilEnable(IL_TYPE_SET);
		ilTypeFunc(IL_UNSIGNED_BYTE);

		ilutRenderer(ILUT_OPENGL);

		ilEnable(IL_FILE_OVERWRITE);

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT1);

		ilHint(IL_MEM_SPEED_HINT, IL_FASTEST);
		ilHint(IL_COMPRESSION_HINT, IL_USE_COMPRESSION);

		ilEnable(IL_NVIDIA_COMPRESS);
		ilutEnable(ILUT_D3D_USE_DXTC);
		ilutEnable(ILUT_D3D_GEN_DXTC);

		iluSetLanguage(ILU_ENGLISH);
	}

	ILuint ImgID = ilGenImage();
	ilBindImage(ImgID);

	auto NameAndExt = StringHelper::Split(LoadingData.m_sFileName, ".");
	if (NameAndExt.size() < 2)
		return;

	std::string sFileName = NameAndExt[0];
	std::string sFileExt = NameAndExt[1];

	ILenum ImType = IL_TYPE_UNKNOWN;
	if (sFileExt == "jpg")
		ImType = IL_JPG;
	else if (sFileExt == "png")
		ImType = IL_PNG;
	else if (sFileExt == "dds")
		ImType = IL_DDS;

	CTextureInitInfo TexInitInfo;

	char *pLoadedData = nullptr;
	size_t nDataSize = 0;

	CResourceManager::GetInstance()->ReadResource(LoadingData.m_pAFCLibrary, LoadingData.m_sFileName, pLoadedData, nDataSize);

	if (ImType != IL_DDS)
	{
		if (!ilLoadL(ImType, pLoadedData, nDataSize))
		{
			ILenum ilError = ilGetError();
			LOG_ERROR("DevIL failed: %s", iluErrorString(ilError));

			CHError er = new CError_DevILError(AT);
			return;
		}

		if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		{
			ILenum ilError = ilGetError();
			LOG_ERROR("DevIL failed: %s", iluErrorString(ilError));

			CHError er = new CError_DevILError(AT);
			return;
		}

		if (MathHelper::NextPowerOf2(ilGetInteger(IL_IMAGE_WIDTH)) != ilGetInteger(IL_IMAGE_WIDTH) ||
			MathHelper::NextPowerOf2(ilGetInteger(IL_IMAGE_HEIGHT)) != ilGetInteger(IL_IMAGE_HEIGHT) ||
			MathHelper::NextPowerOf2(ilGetInteger(IL_IMAGE_DEPTH)) != ilGetInteger(IL_IMAGE_DEPTH))
		{
			if (!iluScale(
				MathHelper::NextPowerOf2(ilGetInteger(IL_IMAGE_WIDTH)),
				MathHelper::NextPowerOf2(ilGetInteger(IL_IMAGE_HEIGHT)),
				MathHelper::NextPowerOf2(ilGetInteger(IL_IMAGE_DEPTH))))
			{
				ILenum ilError = ilGetError();
				LOG_ERROR("DevIL failed: %s", iluErrorString(ilError));

				CHError er = new CError_DevILError(AT);
				return;
			}
		}

		//ilImageToDxtcData(IL_DXT1);
		int nSize = ilGetDXTCData(nullptr, 0, IL_DXT1);
		if (!nSize)
		{
			ILenum ilError = ilGetError();
			LOG_ERROR("DevIL failed: %s", iluErrorString(ilError));

			CHError er = new CError_DevILError(AT);
			return;
		}

		void *pBuffer = malloc(nSize);
		nSize = ilGetDXTCData(pBuffer, nSize, IL_DXT1);
		if (!nSize)
		{
			ILenum ilError = ilGetError();
			LOG_ERROR("DevIL failed: %s", iluErrorString(ilError));

			CHError er = new CError_DevILError(AT);
			return;
		}

		TexInitInfo.nWidth = ilGetInteger(IL_IMAGE_WIDTH);
		TexInitInfo.nHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		TexInitInfo.pData = pBuffer;
		TexInitInfo.sName = LoadingData.m_sFileName;
		TexInitInfo.ePixelFormat = EPixelFormat::BC1;
		TexInitInfo.dwDataSize = nSize;

		m_pTexture2D = CDistributedObjectCreator::GetInstance()->CreateTexture2D();
		m_pTexture2D->VInitTexture(TexInitInfo);

		delete[] pBuffer;

		ilBindImage(0);
		ilDeleteImage(ImgID);
	}
	else
	{
		TexInitInfo.nWidth = 0;
		TexInitInfo.nHeight = 0;
		TexInitInfo.pData = (void *)pLoadedData;
		TexInitInfo.sName = LoadingData.m_sFileName;
		TexInitInfo.ePixelFormat = EPixelFormat::BC1;
		TexInitInfo.dwDataSize = nDataSize;

		m_pTexture2D = CDistributedObjectCreator::GetInstance()->CreateTexture2D();
		m_pTexture2D->VInitTexture(TexInitInfo);
	}

	SAFE_DELETE_ARRAY(pLoadedData);

	g_nNumTextures++;
};

CTextureResource::~CTextureResource()
{
	SAFE_DELETE(m_pTexture2D);

	g_nNumTextures--;
	g_nNumTextures = std::max(0, g_nNumTextures);

	if (g_nNumTextures == 0)
	{
		ilShutDown();
	}
};

CTextureResource::CTextureResource(RESOURCE_ID nResourceID, bool bIsUnique) :
	IResource(nResourceID, bIsUnique)
{
	m_eResourceType = E_RESOURCE_TYPE::TEXTURE;
}