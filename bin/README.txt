Place here the contents of the VAPI project "dll/" folder after building with VAPI_BUILD_DLL=ON on Windows.

Required:
- VAPI DLL (libvapi_shared.dll or vapi.dll)
- vapi_c_api.hpp
- Import library (vapi.lib or libvapi_shared.dll.a)
- shaders/ (ui.vert.spv, ui.frag.spv — or .glsl sources if not yet built)
- vulkan-1.dll
- glfw3.dll (if GLFW built as DLL)

Then build the example in ../examples/ and run your exe from this bin/ (or add bin/ to PATH).
