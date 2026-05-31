# RVault — Next To-Dos

---

## 🟣 ALPHA-1

### Fix `generatePassword()` Logic Bug
**File:** `rvault_ui.cpp`
The "create entry" branch checks `copy` instead of `newEntry`, so entry creation is gated on the clipboard answer.
- [x] Change `if (copy == "y" || copy == "Y")` (second occurrence) to `if (newEntry == "y" || newEntry == "Y")`

### Fix `sodium_memzero` on `std::string` in `generatePassword()`
**File:** `rvault_ui.cpp`
`sodium_memzero(&password, ...)` wipes the string object's metadata, not the character buffer.
- [x] Change to `sodium_memzero(password.data(), password.size())`

---

## 🟣 ALPHA-2

### Fix Vector Memory Zeroing in Destructor
**File:** `rvault_session.cpp`
If the vector reallocated during the session, old heap pages were never zeroed. The current destructor only clears the final allocation.
- [ ] Implement a `sodium_allocator<T>` custom allocator that zeroes memory on deallocation
- [ ] Replace `std::vector<RVaultEntryEncrypted>` with `std::vector<RVaultEntryEncrypted, sodium_allocator<RVaultEntryEncrypted>>`
\
**WORK IN PROGRESS**


### Unify File Handling with RAII
**File:** `rvault_file.cpp`, `rvault_session.cpp`
`RVaultFile` opens/closes a member `fstream` across `create()`, `open()`, and `save()` in inconsistent ways, and `RVaultSession` constructs a local `RVaultFile` on the stack for saving.
- [ ] Refactor `RVaultFile` so each operation owns its file handle locally (open → use → close in one scope)
- [ ] Or convert `RVaultFile` to a scoped RAII wrapper with a clear single-ownership model
- [ ] Investigate atomic save: write to `.tmp` file, then `fs::rename` to final path

---

## 🔴 BETA-1

### Fix Binary Struct Serialization
**File:** `rvault_file.cpp`
Struct padding and alignment differences across compilers/platforms will corrupt vault files. Replace direct `file.write(&header, sizeof(...))` with a portable serialization format.
- [ ] Design a custom TLV (tag-length-value) binary format, or evaluate FlatBuffers
- [ ] Write `serialize_header()` / `deserialize_header()` functions
- [ ] Write `serialize_entry()` / `deserialize_entry()` functions
- [ ] Update `RVaultFile::create()`, `open()`, and `save()` to use new format
- [ ] Test round-trip on Linux and Windows builds

### Add Doxygen Documentation to Headers
All header files are currently undocumented.
- [ ] Set up Doxygen config
- [ ] Document all public functions with `@param`, `@return`, and `@throws`
- [ ] Clarify return code conventions (currently mixed between `-1`, `-2`, `-25`, `-69`)
- [ ] Note thread-safety assumptions (currently none — document that)

---

## 🔴 BETA-2

- [ ] macOS CSPRNG stub in `rvault_random.c` — implement `arc4random_buf()`
- [ ] macOS path support in `rvault_platform.c` — `~/Library/Application Support/rvault/`
- [ ] `getpwuid` fallback for `$HOME` not set on Linux

---

## 🔴 BETA-3

- [ ] 5-minute lockout timer after 5 failed password attempts in `showEntry()` and `deleteVault()`

---

## 🔵 Backlog / Unscheduled

- [ ] Mobile version research/planning
- [ ] Alpha release checklist
