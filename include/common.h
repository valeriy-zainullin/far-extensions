#ifndef COMMON_H
#define COMMON_H

#ifndef TARGET
#error "TARGET" macro is not defined!
#endif

#if TARGET == X86
typedef unsigned int MemoryAddress;
#elif TARGET == X86-64
typedef unsigned long long MemoryAddress;
#else
#error Target is not supported!
#endif

#ifndef NDEBUG
#include <stdio.h>
#include <assert.h>
#define DEBUG_PRINTF(format, arg) assert(fprintf(stderr, format, arg) >= 0)
#define DEBUG_PRINT(format) assert(fprintf(stderr, format))
#else
#define DEBUG_PRINTF(format, arg)
#define DEBUG_PRINT(format)
#endif


#define DLL_EXPORT __declspec(dllexport)
#define EXPLICITLY_UNUSED(variable) ((void) variable)

typedef struct FarColor FarColor;
typedef struct VersionInfo VersionInfo;
typedef struct EditorGetString EditorGetString;
typedef struct EditorInfo EditorInfo;
typedef struct PluginStartupInfo PluginStartupInfo;
typedef struct GlobalInfo GlobalInfo;
typedef struct PluginInfo PluginInfo;
typedef struct OpenInfo OpenInfo;
typedef struct EditorColor EditorColor;
typedef struct ProcessEditorEventInfo ProcessEditorEventInfo;

static const wchar_t* AUTHOR = L"Valeriy Zainullin";

#include <windows.h>
DLL_EXPORT BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	EXPLICITLY_UNUSED(hinstDLL);
	EXPLICITLY_UNUSED(fdwReason);
	EXPLICITLY_UNUSED(lpReserved);
	return TRUE;
}


#endif