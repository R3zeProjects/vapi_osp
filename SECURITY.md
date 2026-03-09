# Security Policy

Copyright (c) 2025–2026 R3zeProjects. This project is licensed under the VAPI OSP Software License (non-commercial free; commercial use by agreement). See [LICENSE](LICENSE) and [licenses/LICENSE_VAPI.txt](licenses/LICENSE_VAPI.txt). The software is provided "AS IS" without warranty; see LICENSE for limitation of liability.

---

## Supported Versions

| Version | Supported |
|---------|-----------|
| 0.x (pre-release / alpha) | Best effort; no long-term support guarantee |
| 1.0+ (future) | Security patches backported to latest minor where feasible |

---

## Reporting a Vulnerability

**Do not report security vulnerabilities in public GitHub Issues.**

Report them privately in one of these ways:

1. **GitHub:** Use [Private vulnerability reporting](https://github.com/R3zeProjects/vapi_osp/security/advisories/new) for this repository (if enabled), or  
2. **Email:** Contact the Creator at the email given in [CONTACT.txt](CONTACT.txt) (Official Channel). Use the subject line **Security report – VAPI OSP** so it can be distinguished from licensing inquiries.

Security reports are welcome from anyone (users, researchers, commercial or non-commercial). Reporting a vulnerability does not require a commercial Agreement and is independent of the license.

Please include:

- Description of the vulnerability and potential impact  
- Steps to reproduce (minimal repro preferred)  
- Affected version(s) (e.g. 0.0.1-alpha)  
- Any suggested mitigation or reference  

You can expect an initial response within **7 business days**. We will work with you to understand the issue and, if accepted, to coordinate disclosure (e.g. advisory and fix).

---

## Security-Related Design Notes

### Shader Loading

- `loadFromFile()` accepts a `LoadShaderOptions` struct with `allowedRoot` (path restriction) and `maxSpirvBytes` (size cap, default 4 MiB).  
- Paths outside `allowedRoot` are rejected. SPIR-V files larger than the cap are rejected before full read.

### File Access

- File utilities do not follow symlinks outside the designated root. Paths are normalized before comparison.

### Third-Party Dependencies

- **GLFW 3.x** — see [GLFW security advisories](https://github.com/glfw/glfw/security).  
- **Vulkan SDK / Vulkan Loader** — see [Khronos security](https://www.khronos.org/registry/vulkan/).  
- **STB libraries** (e.g. stb_truetype) — vendored; update periodically and review upstream.

### Known Limitations

- No input sanitization on text passed to the font renderer (UTF-8 validity is assumed).  
- The `EchoBackend` assistant reflects user input verbatim; do not use in production without filtering.  
- macOS support is experimental; security testing has not been performed on macOS.
