<p align="center">
  <img src="/program_info/org.realmslauncher.RealmsLauncher.logo.png" alt="Realms Launcher">
</p>

<h1 align="center">Realms Launcher</h1>

<p align="center">A Minecraft launcher based on Prism Launcher with support for offline accounts.</p>

<p align="center">
  <a href="https://github.com/RealmsLauncher/RealmsLauncher/actions/workflows/build.yml"><img src="https://github.com/RealmsLauncher/RealmsLauncher/actions/workflows/build.yml/badge.svg" alt="Build"></a>
  <a href="https://github.com/RealmsLauncher/RealmsLauncher/releases"><img src="https://img.shields.io/github/v/release/RealmsLauncher/RealmsLauncher?display_name=tag&sort=semver" alt="Latest release"></a>
  <a href="https://github.com/RealmsLauncher/RealmsLauncher/blob/main/LICENSE"><img src="https://img.shields.io/badge/license-GPL--3.0-blue.svg" alt="License"></a>
</p>

Realms Launcher is a fork of [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher). It keeps the instance management and modding workflow of Prism Launcher while adding changes made for this project, including offline account support.

Realms Launcher is not affiliated with or endorsed by Mojang Studios, Microsoft, or Prism Launcher.

## Features

- Offline accounts
- Microsoft account support
- Multiple Minecraft instances
- Mod loaders, mods, resource packs, worlds, and separate instance settings
- Java runtime management and Java downloading
- Linux, Windows, and macOS builds
- ARM64 builds where supported
- CMake presets for local development
- Automated builds and releases through GitHub Actions
- GNU GPL v3.0 licensed

## Downloads

Builds are published on the [Releases](https://github.com/RealmsLauncher/RealmsLauncher/releases) page when available.

| Platform | Builds |
| --- | --- |
| Linux | x86_64, ARM64 |
| Windows | x86_64, ARM64; MSVC and MinGW |
| macOS | Intel, Apple Silicon |

Development and release builds can also be found in GitHub Actions when they are produced by the workflow.

### Offline accounts

Offline accounts are local player profiles and do not authenticate with Microsoft or Mojang services. They can be useful for offline play and for servers that explicitly allow offline-mode players.

If a server requires a properly authenticated Minecraft account, use a Microsoft account instead. Offline accounts should not be used to impersonate another player or get around a server's authentication system.

## Building

The project uses CMake, Ninja, Qt 6, and C++20. CMake presets are provided for the supported platforms.

See [`docs/BUILDING.md`](docs/BUILDING.md) for dependency and platform-specific setup instructions.

A basic Linux build looks like this:

```bash
cmake --preset linux
cmake --build --preset linux --config Release
ctest --preset linux --build-config Release
```

Available presets:

```text
linux
macos
macos_arm64
macos_x86_64
macos_universal
windows_mingw
windows_msvc
```

The project uses an out-of-source build directory. In-source builds are intentionally rejected by the CMake configuration.

## CI

The release workflow builds and tests the launcher for:

- Linux x86_64
- Linux ARM64
- Windows x86_64 (MSVC)
- Windows x86_64 (MinGW/Clang64)
- Windows ARM64 (MSVC)
- Windows ARM64 (MinGW/ClangARM64)
- macOS Intel
- macOS Apple Silicon

The workflow is in [`.github/workflows/build.yml`](.github/workflows/build.yml), with reusable build steps in [`.github/actions/`](.github/actions/).

## Repository layout

```text
launcher/             Main launcher application
libraries/            Project libraries and integrations
cmake/                CMake modules and build helpers
program_info/         Application information and assets
tests/                Automated tests
.github/actions/      Reusable GitHub Actions
.github/workflows/    CI and release workflows
CMakeLists.txt        Main CMake configuration
CMakePresets.json     Local build presets
CONTRIBUTING.md       Contribution guidelines
LICENSE               GNU GPL v3.0
README.md             Project information
```

## Contributing

Bug fixes, improvements, and other contributions are welcome.

Before submitting a pull request, please read [`CONTRIBUTING.md`](CONTRIBUTING.md). Keep changes focused, run the relevant tests and formatting tools, and explain what the change does and why it is needed.

AI tools can be used to help with development, but contributors are responsible for the code they submit. See [`CONTRIBUTING.md`](CONTRIBUTING.md) for the project's requirements around AI-assisted changes and commit attribution.

## Issues

Use the issue tracker for bugs, feature requests, and larger design proposals.

- [Bug report](https://github.com/RealmsLauncher/RealmsLauncher/issues/new?template=bug_report.yml)
- [Feature request](https://github.com/RealmsLauncher/RealmsLauncher/issues/new?template=suggestion.yml)
- [RFC](https://github.com/RealmsLauncher/RealmsLauncher/issues/new?template=rfc.yml)
- [All issues](https://github.com/RealmsLauncher/RealmsLauncher/issues)

For bug reports, include the launcher version, operating system, steps to reproduce the problem, and relevant logs. Do not post passwords, access tokens, or other private credentials.

## License

Realms Launcher is licensed under the [GNU General Public License v3.0](LICENSE).

This project is based on [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher). See the repository's license and source files for applicable upstream notices.

## Credits

Thanks to the [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher) contributors and the open-source Minecraft community.

---

<p align="center">
  <sub>Realms Launcher</sub>
</p>
