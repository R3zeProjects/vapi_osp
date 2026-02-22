#!/usr/bin/env python3
"""
Minimal VAPI example (Python 3.14+).
Run from repo root with bin/ on PATH and bin/ on PYTHONPATH:
  set PATH=%CD%\bin;%PATH%
  set PYTHONPATH=%CD%\bin
  python examples/main.py
"""
from __future__ import annotations

import os
import sys

# Ensure bin/ is on path for vapi and for DLLs
_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
_bin = os.path.join(_root, "bin")
if _bin not in sys.path:
    sys.path.insert(0, _bin)
os.environ.setdefault("PATH", "")
if _bin not in os.environ["PATH"]:
    os.environ["PATH"] = _bin + os.pathsep + os.environ["PATH"]

from vapi import App, AppConfig, Worksheet


def main() -> int:
    with App() as app:
        app.init(AppConfig(
            title="VAPI Worksheet (Python)",
            width=1280,
            height=720,
            validation=True,
            vsync=True,
        ))
        app.push_layer(Worksheet.create())
        app.run()
    return 0


if __name__ == "__main__":
    sys.exit(main())
