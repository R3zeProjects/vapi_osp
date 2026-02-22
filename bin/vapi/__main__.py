"""
Run the minimal VAPI example: python -m vapi
Run from repo root with: set PATH=%CD%\bin;%PATH%  and  set PYTHONPATH=%CD%\bin
"""
from __future__ import annotations

import sys

def _run() -> int:
    from . import App, AppConfig, Worksheet
    with App() as app:
        app.init(AppConfig(title="VAPI Worksheet (Python)", width=1280, height=720))
        app.push_layer(Worksheet.create())
        app.run()
    return 0

def main() -> int:
    """Entry point for the ``vapi`` console script (pip install)."""
    return _run()


if __name__ == "__main__":
    sys.exit(_run())
