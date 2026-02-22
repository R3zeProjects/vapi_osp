/*
 * Minimal VAPI C API example (Windows).
 * Build: link with bin/vapi.lib (MSVC) or -lvapi_shared (MinGW), include path = bin/
 * Run: from bin/ or add bin/ to PATH (DLL + shaders/ required).
 */
#include "vapi_c_api.hpp"
#include <stdlib.h>

int main(void)
{
    VAPI_App app = VAPI_AppCreate();
    if (!app)
        return 1;

    VAPI_AppConfig cfg = {0};
    cfg.title      = "VAPI Worksheet (C)";
    cfg.width      = 1280;
    cfg.height     = 720;
    cfg.validation = 1;
    cfg.vsync      = 1;

    if (VAPI_AppInit(app, &cfg) != 0) {
        VAPI_AppDestroy(app);
        return 1;
    }

    VAPI_Layer ws = VAPI_WorksheetCreate();
    if (ws)
        VAPI_AppPushLayer(app, ws);

    VAPI_AppRun(app);
    VAPI_AppDestroy(app);
    return 0;
}
