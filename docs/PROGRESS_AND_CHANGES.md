# Progress and changes (documentation and repository)

This document records **recent documentation and repository updates** made to align author identity, release notes, and project hygiene. It is intended to make the progress visible to contributors and maintainers.

---

## 1. Author identity (SHA3-512)

**What was done**

- The project exposes an `author` constant in `include/core/types.hpp` (namespace `vapi`, inline `std::string_view author`). It is intended to identify the creator in a stable, opaque form.
- The value was updated to the **SHA3-512 hash** of the canonical author email **`r3ze.projects@gmail.com`**, so that:
  - The same identity is used in Git config (`user.name` = `r3ze`, `user.email` = `r3ze.projects@gmail.com`).
  - There is no mismatch between “author in code” and “author in version control,” which avoids legal/attribution ambiguity.

**Exact value**

- **Hash algorithm:** SHA3-512 (Keccak-based, 512-bit).
- **Input:** the string `r3ze.projects@gmail.com` (no trailing newline or extra characters).
- **Output (hex):**  
  `bcc2eecba5ed7cb5bfd489f52db02fe344297ec4de521e73b2fab09abc5fe89692998ac284d6a441d51669bb06cd8dea5671810fb1973bc7fa8f954d32053545`

**File changed**

- `include/core/types.hpp`: the `author` constant was set to the hex string above (split across two string literals for line length).

**Verification**

- The value was checked with Python:  
  `hashlib.sha3_512(b'r3ze.projects@gmail.com').hexdigest()`  
  matches the constant in the source.

---

## 2. `.gitignore`: exclude `oldvapi/`

**What was done**

- The directory (or path) **`oldvapi/`** was added to `.gitignore` so that legacy or private “old VAPI” content is not tracked by Git.

**File changed**

- `.gitignore`: under the section “Personal / workspace notes (not for public repo),” the line `oldvapi/` was added.

---

## 3. Release notes and version comparison

**What was done**

- A single file **`RELEASE_NOTES.md`** at the repository root was created (and later extended) to hold:
  - **Version logic:** See [VERSIONING.md](VERSIONING.md). Scheme: `0.` = released, `0.0` = beta, `0.0.1` = alpha, `b` = patch. **Current API version:** 0.0.2b3; in CMake: 0.0.2. Sequence: 0.0.1-alpha → 0.0.1b2 → 0.0.2b1 → 0.0.2b2 → 0.0.2b3 (current) → 0.1.0 (first stable).
  - **Current API version 0.0.2b3:** Type safety & Linux build release; set in `CMakeLists.txt` as 0.0.2.

**Purpose**

- To have ready-to-use **copy-paste text for publishing** each version (version number, short description, list of improvements).
- To make **progress between versions** explicit (e.g. 0.1.0 vs 0.2.0) in one place.

**File created/updated**

- `RELEASE_NOTES.md`: version logic and current 0.0.1-alpha (first alpha), including announcement text.

---

## 4. This document (`docs/PROGRESS_AND_CHANGES.md`)

**What was done**

- This file was added to the documentation to describe, in English and in detail:
  - The author identity change (SHA3-512 of `r3ze.projects@gmail.com`).
  - The `.gitignore` update for `oldvapi/`.
  - The creation and content of `RELEASE_NOTES.md` (current 0.0.1-alpha, first alpha).

**Purpose**

- To make it clear **what was changed** and **why**, so that progress is visible and the setup is reproducible (e.g. verifying the author hash, understanding release content).

---

## Summary table

| Item | Location | Change |
|------|----------|--------|
| Author constant | `include/core/types.hpp` | Set to SHA3-512(`r3ze.projects@gmail.com`) hex value |
| Ignore path | `.gitignore` | Added `oldvapi/` |
| Current API version 0.0.2b3 | CMakeLists.txt, RELEASE_NOTES.md, docs/VERSIONING.md | Current = 0.0.2b2; CMake 0.0.2 |
| Progress log | `docs/PROGRESS_AND_CHANGES.md` | This document (detailed English description of the above) |

---

## 5. Documentation and comments in English

**What was done**

- All primary documentation and comments were converted to English so that the project standard is English-only (except explicitly localized files like `*.ru.md`).
- **Translated to English:** RELEASE_NOTES.md, docs/VERSIONING.md, docs/RELEASING.md, docs/README.md, README.md, RELEASE_CHECKLIST.md, docs/integration.md, docs/quick_start.md, docs/user_guide.md, docs/architecture.md, docs/testing.md, docs/quick_reference.md, docs/usage_examples.md, examples/README.md, test_glfw_cmake/CMakeLists.txt; comments in root CMakeLists.txt, include/core/error.hpp, include/render/font_render_app.hpp; include/core/README.md (one phrase); removed stray lines in include/core/types.hpp.
- **Added:** docs/DOCUMENTATION_LANGUAGE.md — states that all docs and code comments are in English and lists exceptions (e.g. security_layer.ru.md, learning.ru.md). Notes that some older docs and sources may still have Russian and should be translated when edited.

---

## Optional: GPG signing (not applied in repo)

Discussed but **not** changed in the repository:

- Using **GPG** for signed commits (with `user.name` = `r3ze`, `user.email` = `r3ze.projects@gmail.com`) so that GitHub/GitLab show “Verified.”
- Preferring **ECC (sign-only)** and **SHA-512** for signing where supported.
- Configuring `gpg.conf` with `personal-digest-preferences SHA512 ...` and `cert-digest-algo SHA512` so signatures use SHA-512.

No config files or scripts for GPG were added; the above is for local setup only.
