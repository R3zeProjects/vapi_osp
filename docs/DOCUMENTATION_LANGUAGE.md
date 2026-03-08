# Documentation and comment language

All project **documentation** and **code comments** are in **English**.

- Root and `docs/` markdown files are written in English.
- Comments in CMake (CMakeLists.txt, presets) are in English.
- Comments in C++ source and headers are in English.

**Exceptions:** Optional localized docs keep a language suffix and are not the primary reference:

- `docs/security_layer.ru.md` — security layer (Russian).
- `docs/render/learning.ru.md` — render learning (Russian).

When adding or editing docs or comments, use English. For user-facing text (UI strings, log messages) the project may define localization later; until then, English is preferred.

**Migration note:** Some older docs (e.g. full_documentation.md, scaling_and_changes.md, font_ui_text.md, api_*.md, descriptor_pipeline_link.md, detail_docs/platform.md) and some source files (e.g. in include/font/, include/platform/, src/) may still contain Russian. Translate them to English when you touch those files.
