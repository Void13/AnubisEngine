#ifndef H_ENGINEPCH
#define H_ENGINEPCH

#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable: 4275)
#pragma warning(disable: 4099)

#include <Windows.h>

#include <assert.h>
#include <memory.h>

#include <codecvt>

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <set>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <ctime>

#include <thread>
#include <mutex>
#include <atomic>
#include <type_traits>

#include <memory>
#include <stdarg.h>

#include "CEGUI\\CEGUI.h"
#include "CEGUI\\System.h"
#include "CEGUI\\DefaultResourceProvider.h"
#include "CEGUI\\ImageManager.h"
#include "CEGUI\\Image.h"
#include "CEGUI\\Font.h"
#include "CEGUI\\Scheme.h"
#include "CEGUI\\WindowManager.h"
#include "CEGUI\\ScriptModule.h"
#include "CEGUI\\XMLParser.h"
#include "CEGUI\\GeometryBuffer.h"
#include "CEGUI\\GUIContext.h"
#include "CEGUI\\RenderTarget.h"
#include "CEGUI\\AnimationManager.h"

#define _XM_NO_INTRINSICS_ 1

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include "DirectXMath\\DirectXSH.h"
#include "DirectXMath\\DirectXColor.h"

#undef GetObject

#define USE_D3D11 1
#define USE_WINDOWS 1

using namespace DirectX;

#if defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define SAFE_RELEASE(x) { if ((x)) { if ((x)->Release()) { } } (x) = nullptr; }
#define SAFE_DELETE(x) { if ((x)) { delete (x); } (x) = nullptr; }
#define SAFE_DELETE_ARRAY(x) { if ((x)) { delete[] (x); } (x) = nullptr; }

#define INI_ERROR "er53fs"

//#define DEBUGBRAKE() __asm { int 3 }
#define DEBUGBRAKE() __debugbreak()

#include "DistributedObjectCreator.h"
#include "Strings.h"
#include "LogManager.h"
#include "ErrorHandler.h"
#include "HRTimer.h"
#include "EventManager.h"
#include "EngineInputEvents.h"
#include "MathHelper.h"
#include "IntersectionMath.h"
#include "CLangHelper.h"
#include "SettingsHandler.h"

#include "FileHelper.h"
#include "FINI.h"

#include "AFCLibrary.h"
#include "LerpTable.h"

#include "ResourceManager\\ResourceManager.h"
#include "ResourceManager\\Resource.h"
#include "ResourceManager\\Resource_Model.h"
#include "ResourceManager\\Resource_Texture.h"

#ifdef _DEBUG
	#pragma comment(lib, "AFCLibraryD.lib")
#else
	#pragma comment(lib, "AFCLibrary.lib")
#endif

#endif