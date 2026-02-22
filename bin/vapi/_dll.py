"""
Low-level ctypes bindings to libvapi_shared.dll (VAPI C API).
Package lives in bin/; DLL is loaded from the same directory (bin/).
"""
from __future__ import annotations

import os
import sys
from ctypes import (
    CDLL,
    Structure,
    c_char_p,
    c_int,
    c_void_p,
    POINTER,
)

def _dll_dir() -> str:
    """Directory containing libvapi_shared.dll (bin/ when adapter is in bin/vapi/)."""
    # __file__ = .../bin/vapi/_dll.py -> .../bin
    return os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def _load_dll():
    if sys.platform != "win32":
        raise RuntimeError("VAPI Python bindings support Windows only.")
    dll_dir = _dll_dir()
    try:
        os.add_dll_directory(dll_dir)
    except AttributeError:
        pass
    os.environ.setdefault("PATH", "")
    if dll_dir not in os.environ["PATH"]:
        os.environ["PATH"] = dll_dir + os.pathsep + os.environ["PATH"]
    return CDLL("libvapi_shared.dll")


class VAPI_AppConfig(Structure):
    _fields_ = [
        ("title", c_char_p),
        ("width", c_int),
        ("height", c_int),
        ("validation", c_int),
        ("vsync", c_int),
    ]


VAPI_App = c_void_p
VAPI_Layer = c_void_p

_dll = _load_dll()

VAPI_AppCreate = _dll.VAPI_AppCreate
VAPI_AppCreate.argtypes = []
VAPI_AppCreate.restype = VAPI_App

VAPI_AppDestroy = _dll.VAPI_AppDestroy
VAPI_AppDestroy.argtypes = [VAPI_App]
VAPI_AppDestroy.restype = None

VAPI_AppInit = _dll.VAPI_AppInit
VAPI_AppInit.argtypes = [VAPI_App, POINTER(VAPI_AppConfig)]
VAPI_AppInit.restype = c_int

VAPI_AppRun = _dll.VAPI_AppRun
VAPI_AppRun.argtypes = [VAPI_App]
VAPI_AppRun.restype = None

VAPI_AppPushLayer = _dll.VAPI_AppPushLayer
VAPI_AppPushLayer.argtypes = [VAPI_App, VAPI_Layer]
VAPI_AppPushLayer.restype = None

VAPI_WorksheetCreate = _dll.VAPI_WorksheetCreate
VAPI_WorksheetCreate.argtypes = []
VAPI_WorksheetCreate.restype = VAPI_Layer

VAPI_CheckDebugger = _dll.VAPI_CheckDebugger
VAPI_CheckDebugger.argtypes = []
VAPI_CheckDebugger.restype = c_int
