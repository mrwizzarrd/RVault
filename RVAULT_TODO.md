# RVault Pre-Alpha TODO

## 🔴 Bugs (Fix First)
- [x] **Wrong sizeof in `addNewEntry()`** — `memset(&newEntry, 0, sizeof(RVaultEntryPlain))` should be `sizeof(RVaultEntryEncrypted)`

---

## 🟠 Unimplemented Features
- [x] **Show Entry Password** — Case 4 in main menu is a stub
- [ ] **Delete Vault** — Case 5 in main menu is a stub
- [ ] **Generate Password** — Case 6 in main menu is a stub
- [x] **`getEntry()`** — declared in `rvault_session.h` but never implemented in `rvault_session.cpp`

---

## 🟠 Path Handling
- [ ] Replace hardcoded `TESTING_PATH` (`../testFiles/testing.rvault`) with real platform-aware path resolution
  - Use `rvault_get_platform()` and `rvault_get_username()` — you already wrote these, use them
  - Store vault in appropriate user directory per platform (e.g. `~/.local/share/rvault/` on Linux, `%APPDATA%\rvault\` on Windows)

---t

## 🟡 Platform Issues
- [ ] **Windows — silent password input broken** — `quietPrompt()` returns `""` on Windows, needs implementation using `ReadConsoleW` with `ENABLE_ECHO_INPUT` disabled
- [ ] **macOS — CSPRNG is empty** — `rvault_random.c` macOS branch does nothing, implement with `arc4random_buf()`
- [ ] **Non-portable header** — `#include <bits/local_lim.h>` in `rvault_platform.c` is a GCC internal, replace with `<limits.h>`

---

## 🟡 Cross-Platform File Compatibility
- [ ] `std::time_t` in `RVaultHeader` is written raw to disk — its size is platform-dependent, consider using a fixed-width type like `int64_t` to avoid cross-platform file corruption

---

## 🟢 Code Quality
- [ ] `removeEntry()` in `rvault_session.cpp` heap-allocates `RVaultEntryPlain` with `new` unnecessarily — switch to stack allocation
- [ ] `INVALID_PASSWORD_ERROR` and `GENERIC_ERROR` constants in `rvault_constants.h` are dead code now that exceptions are used — remove them

---

## 🟢 Before Handing to Testers
- [ ] Write README (what it is, how to build, basic usage, crypto stack)
- [ ] Build instructions for Linux and Windows
- [ ] Verify hardcoded path is fully replaced before any external testing
