# RVault
# Version : Pre-Alpha 4
## DISCLAIMER : This is pre-alpha software, not recommended for storing real passwords yet

## Project description
A password manager that uses secure file storage and random generation for creating and storing passwords. Uses libsodium for encryption for secure file storage using the AEAD XChaCha20-Poly1305 cryptography algorithm, as well as Argon2 for key derivation.

## Dependencies
- [Libsodium](https://libsodium.gitbook.io/doc)- The main crypto library
- [Clip](https://github.com/dacap/clip)- Used for copy/paste utility 

## Build tools
- CMake- Main Project compilation handler
- Ninja- Used for compilation


## How to build

### Linux
**Clone Repository** \
`git clone --recursive https://github.com/rolandvermiglio/RVault`

**Install Dependencies** 
- Debian based- `sudo apt install libsodium-dev cmake ninja-build`
- Red Hat based- `sudo dnf install libsodium-devel cmake ninja-build`
- Arch based- `sudo pacman -S libsodium cmake ninja`

**Build and Compile** \
`mkdir build && cd build`
`cmake .. \
-G Ninja`
`ninja`
`./rvault`

### Windows
**Install** [MSYS2](https://www.msys2.org)

**In the MSYS2 MinGW 64-bit shell** \
`pacman -S mingw-w64-x86_64-libsodium mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-git`
`git clone --recursive https://github.com/rolandvermiglio/RVault`

**Build and Compile** \
`mkdir build && cd build`
`cmake .. \
-G Ninja`
`ninja`
`./rvault.exe`

## Basic usage
**NOTE: When entering any password- It will not show up as being there, but it will be there**


Upon opening for the first time, or if no vault file is detected, you will be prompted to provide your name (For a future display label) and a master password

`No User Detected, Initiating First Time Setup
Name: Example Name
Enter Master Password:`

Upon opening after the first time, you will only be prompted for a master password
`Enter Master Password:`

The main screen will give 7 options:
`RVault
Options:
1 - List Entries
2 - Add Entry
3 - Remove Entry
4 - Show Entry
5 - Delete Vault
6 - Generate Password
7 - Exit
Input >`

**Notes:**
- When you want an entry shown you will be asked for your master password
- When you generate a password you can copy to clipboard and make a new vault entry with that password
- **On LINUX : File is stored in `~/.local/share/rvault/` and is named vault.rvault**
- **On WINDOWS : File is stored in `%LOCALAPPDATA%\rvault` and is named vault.rvault**

## Changelog

Alpha-1
  - Bug fixes regarding UI 

## License : GNU GPLv3
