# App: Application, AppConfig, AppLayer

Main application class, configuration and layers. Headers: app/application.hpp, app/app_config.hpp, app/app_layer.hpp.

## Application

**init(const AppConfig& cfg)** — initializes platform, GPU and UI. Returns Result<void>. **run()** — blocking main loop. **quit()** — set exit flag. **pushLayer(unique_ptr<AppLayer>)** — add layer. **popLayer(name)** — remove layer by name.

Access: **platform()**, **mainWindowId()**, **input()**, **instance()**, **device()**, **surface()**, **swapchain()**, **renderCtx()**, **uiManager()**, **clock()**. **width()**, **height()**, **aspectRatio()**, **isMinimized()**, **setTitle(title)**.

## AppConfig

title, width, height, resizable, fullscreen, decorated, validation, vsync, framesInFlight, fixedTimestep, maxDeltaTime, showFps.

## AppLayer

Base class for layers. Virtual methods: **onAttach(app)**, **onDetach()**, **onUpdate(dt)**, **onFixedUpdate(fixedDt)**, **onRender()**, **onUIRender()**, **onEvent()** (true = event consumed), **getPreUIBatch()**. **name()**, **isEnabled()**, **setEnabled(bool)**.

## C API

Use **VAPI_AppCreate()**, **VAPI_AppInit(app, &cfg)**, **VAPI_AppPushLayer(app, layer)**, **VAPI_AppRun(app)**. See 05-c-api.md.
