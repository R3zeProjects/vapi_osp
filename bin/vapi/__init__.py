"""
VAPI Python bindings (Python 3.14+).
Thin wrapper around the VAPI C API (libvapi_shared.dll). Package lives in bin/.
"""
from __future__ import annotations

from ._dll import (
    VAPI_AppConfig as _AppConfig,
    VAPI_App,
    VAPI_Layer,
    VAPI_AppCreate,
    VAPI_AppDestroy,
    VAPI_AppInit,
    VAPI_AppRun,
    VAPI_AppPushLayer,
    VAPI_WorksheetCreate,
    VAPI_CheckDebugger,
)
from ctypes import byref

__all__ = [
    "App",
    "AppConfig",
    "Worksheet",
    "check_debugger",
]


def check_debugger() -> bool:
    """Return True if a debugger is attached."""
    return bool(VAPI_CheckDebugger())


class AppConfig:
    """Configuration for VAPI application (title, size, validation, vsync)."""

    __slots__ = ("title", "width", "height", "validation", "vsync")

    def __init__(
        self,
        title: str = "VAPI",
        width: int = 1280,
        height: int = 720,
        validation: bool = True,
        vsync: bool = True,
    ):
        self.title = title
        self.width = width
        self.height = height
        self.validation = validation
        self.vsync = vsync


class App:
    """VAPI application. Use as context manager or call destroy() when done."""

    __slots__ = ("_handle",)

    def __init__(self):
        self._handle: VAPI_App = VAPI_AppCreate()
        if not self._handle:
            raise RuntimeError("VAPI_AppCreate failed")

    def __enter__(self) -> "App":
        return self

    def __exit__(self, *args) -> None:
        self.destroy()

    def init(self, config: AppConfig | None = None) -> None:
        """Initialize the app. Raises RuntimeError on failure."""
        config = config or AppConfig()
        title_bytes = config.title.encode("utf-8")
        cfg = _AppConfig(
            title=title_bytes,
            width=config.width,
            height=config.height,
            validation=1 if config.validation else 0,
            vsync=1 if config.vsync else 0,
        )
        if VAPI_AppInit(self._handle, byref(cfg)) != 0:
            raise RuntimeError("VAPI_AppInit failed")

    def push_layer(self, layer: "Worksheet") -> None:
        """Add a layer (e.g. Worksheet). Ownership is transferred to the app."""
        VAPI_AppPushLayer(self._handle, layer._handle)

    def run(self) -> None:
        """Run the main loop (blocking until window is closed)."""
        VAPI_AppRun(self._handle)

    def destroy(self) -> None:
        """Release the application. Safe to call once."""
        if self._handle:
            VAPI_AppDestroy(self._handle)
            self._handle = None


class Worksheet:
    """Worksheet layer (canvas with blocks). Pass to App.push_layer()."""

    __slots__ = ("_handle",)

    def __init__(self):
        self._handle: VAPI_Layer = VAPI_WorksheetCreate()
        if not self._handle:
            raise RuntimeError("VAPI_WorksheetCreate failed")

    @staticmethod
    def create() -> "Worksheet":
        """Create a new Worksheet layer."""
        return Worksheet()
