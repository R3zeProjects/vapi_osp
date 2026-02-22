#ifndef VAPI_APP_VAPI_C_API_HPP
#define VAPI_APP_VAPI_C_API_HPP

/**
 * Minimal C API for VAPI DLL. Only these symbols are exported;
 * internal C++ symbols stay hidden (protection).
 */

#ifdef _WIN32
    #ifdef VAPI_DLL_BUILD
        #define VAPI_C_API __declspec(dllexport)
    #else
        #define VAPI_C_API __declspec(dllimport)
    #endif
#else
    #define VAPI_C_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* VAPI_App;
typedef void* VAPI_Layer;

typedef struct VAPI_AppConfig {
    const char* title;
    int width;
    int height;
    int validation;  /* 1 = true */
    int vsync;       /* 1 = true */
} VAPI_AppConfig;

VAPI_C_API VAPI_App    VAPI_AppCreate(void);
VAPI_C_API void        VAPI_AppDestroy(VAPI_App app);
VAPI_C_API int         VAPI_AppInit(VAPI_App app, const VAPI_AppConfig* cfg);
VAPI_C_API void        VAPI_AppRun(VAPI_App app);
VAPI_C_API void        VAPI_AppPushLayer(VAPI_App app, VAPI_Layer layer);

VAPI_C_API VAPI_Layer  VAPI_WorksheetCreate(void);

/** Returns 1 if debugger present (optional protection). */
VAPI_C_API int         VAPI_CheckDebugger(void);

#ifdef __cplusplus
}
#endif

#undef VAPI_C_API

#endif
