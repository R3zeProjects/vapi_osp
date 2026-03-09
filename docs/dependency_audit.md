# Dependency audit (VAPI OSP)

Periodic checks of third-party dependencies for known vulnerabilities (CVE) and updates.

## GLFW

- **Website:** https://www.glfw.org/
- **Releases:** https://github.com/glfw/glfw/releases
- **Security:** Check GitHub advisories: https://github.com/glfw/glfw/security/advisories
- **Action:** Update to a supported version when a CVE is announced; set `GLFW3_ROOT` or system package to the new version and rebuild.

## Vulkan SDK / Loader

- **LunarG SDK:** https://vulkan.lunarg.com/sdk/home
- **Vulkan Loader (Khronos):** https://github.com/KhronosGroup/Vulkan-Loader
- **Advisories:** https://github.com/KhronosGroup/Vulkan-Loader/security/advisories
- **Action:** Keep SDK/loader updated; use a version that receives security fixes.

## glslangValidator (optional)

- **Shader compilation:** Part of Vulkan SDK or https://github.com/KhronosGroup/glslang
- Used for compiling GLSL to SPIR-V at build time (examples). Not required at runtime.
- **Action:** Update with Vulkan SDK or glslang releases.

## Suggested workflow

1. **Before release:** Check GLFW and Vulkan-Loader security advisories for the versions in use.
2. **CI (optional):** Add a job that runs `cve-check-tool` or similar on vendored/specified versions, or links to the advisory pages above.
3. **Document versions:** In `docs/VERSIONING.md` or here, note the minimum or recommended GLFW/Vulkan versions that have been audited.

## See also

- `docs/security_layer.ru.md` for project-specific security notes.
