# Building Realms Launcher

This guide covers building Realms Launcher locally from source. The project uses CMake, Ninja, Qt 6, and C++20.

## Prerequisites

You need:

- **Git** with submodule support
- **CMake 3.28 or newer** (recommended for the included presets)
- **Ninja**
- A C++20-capable compiler/toolchain
- **Qt 6** and the dependencies used by the launcher
- **ECM** (Extra CMake Modules)
- **vcpkg** for the macOS and MSVC presets, with `VCPKG_ROOT` configured

The exact system packages vary by operating system. The repository's GitHub Actions setup is a useful reference for the CI dependency setup in `.github/actions/setup-dependencies/`.

## Clone the repository

Clone with submodules because the build depends on repository submodules:

```bash
git clone --recurse-submodules https://github.com/RealmsLauncher/RealmsLauncher.git
cd RealmsLauncher
```

If you already cloned without submodules:

```bash
git submodule update --init --recursive
```

## CMake presets

The repository provides presets for the supported platforms:

| Preset | Target |
| --- | --- |
| `linux` | Linux host |
| `macos` | macOS host |
| `macos_arm64` | macOS Apple Silicon |
| `macos_x86_64` | macOS Intel |
| `macos_universal` | Universal macOS binary |
| `windows_mingw` | Windows with MinGW/Clang |
| `windows_msvc` | Windows with MSVC |

The presets use an out-of-source `build/` directory automatically.

## Linux

Configure, build, and test:

```bash
cmake --preset linux
cmake --build --preset linux --config Release
ctest --preset linux --build-config Release
```

For a debug build:

```bash
cmake --build --preset linux --config Debug
ctest --preset linux --build-config Debug
```

## macOS

### Apple Silicon

```bash
cmake --preset macos_arm64
cmake --build --preset macos_arm64 --config Release
ctest --preset macos_arm64 --build-config Release
```

### Intel

```bash
cmake --preset macos_x86_64
cmake --build --preset macos_x86_64 --config Release
ctest --preset macos_x86_64 --build-config Release
```

### Universal

```bash
cmake --preset macos_universal
cmake --build --preset macos_universal --config Release
ctest --preset macos_universal --build-config Release
```

The macOS presets use vcpkg and configure the appropriate architecture automatically. Make sure `VCPKG_ROOT` points to your vcpkg installation.

## Windows

### MSVC

Use a Visual Studio developer environment with the required C++ workload and vcpkg configured through `VCPKG_ROOT`:

```powershell
cmake --preset windows_msvc
cmake --build --preset windows_msvc --config Release
ctest --preset windows_msvc --build-config Release
```

### MinGW / Clang

From the appropriate MSYS2/MinGW environment:

```bash
cmake --preset windows_mingw
cmake --build --preset windows_mingw --config Release
ctest --preset windows_mingw --build-config Release
```

The CI pipeline uses separate Windows configurations for x86_64 and ARM64. Local architecture support depends on the compiler/toolchain installed on the machine.

## Build options

The main CMake configuration exposes several useful options.

### AddressSanitizer

Enable AddressSanitizer/UndefinedBehaviorSanitizer for debug-oriented builds:

```bash
cmake --preset linux -DDEBUG_ADDRESS_SANITIZER=ON
```

### clang-tidy

Enable clang-tidy when the tool is installed:

```bash
cmake --preset linux -DUSE_CLANG_TIDY=ON
```

### Link-time optimization

Release-oriented presets already enable LTO through the base preset. For a custom configuration, the project exposes:

```text
-DENABLE_LTO=ON
```

## Development workflow

A typical development loop is:

```bash
# Configure once (or whenever CMake configuration changes)
cmake --preset linux

# Build
cmake --build --preset linux --config Debug

# Run tests
ctest --preset linux --build-config Debug
```

Before opening a pull request, also run clang-format on changed C++ files and review the complete diff.

See [`CONTRIBUTING.md`](../CONTRIBUTING.md) for the project's coding conventions, DCO requirements, AI-assisted development policy, and contribution process.

## Continuous integration

The main release workflow is `.github/workflows/build.yml`. It configures the project with the same CMake presets used by local development, builds each platform target, runs tests, and packages successful builds.

The current CI matrix includes:

- Linux x86_64
- Linux ARM64
- Windows x86_64 / MSVC
- Windows x86_64 / MinGW
- Windows ARM64 / MSVC
- Windows ARM64 / MinGW
- macOS Intel
- macOS Apple Silicon

## Troubleshooting

### CMake says the build is in-source

Do not run CMake directly in the repository root with the source directory as the build directory. Use one of the supplied presets instead.

If you created an accidental in-source build, remove the generated build files and start again with a preset.

### Dependencies cannot be found

Check that the required development packages are installed and that environment variables such as `VCPKG_ROOT` are configured where required. Compare your setup with `.github/actions/setup-dependencies/` and the platform-specific packaging actions under `.github/actions/package/`.

### Submodule errors

Refresh the repository's submodules:

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

### Qt or CMake configuration errors

Make sure the compiler, CMake, Ninja, Qt, ECM, and vcpkg versions are compatible with the selected preset. Delete the local `build/` directory and reconfigure if you changed toolchains or dependency locations.

## Reproducible CI builds

The GitHub Actions workflow is the canonical automated build environment for official release artifacts. It pins the runner families and records the Qt/toolchain setup used by each platform job.

For release artifacts, prefer the workflow rather than manually distributing locally built binaries.
