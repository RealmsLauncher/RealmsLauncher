<p align="center">
  <img src="/program_info/org.realmslauncher.RealmsLauncher.logo.png" alt="Realms Launcher" width="128">
</p>

<h1 align="center">Realms Launcher</h1>

<p align="center">
  A modern, open-source Minecraft launcher focused on flexibility, offline accounts, and a familiar Prism Launcher-based experience.
</p>

<p align="center">
  <a href="https://github.com/RealmsLauncher/RealmsLauncher/actions/workflows/build.yml"><img src="https://github.com/RealmsLauncher/RealmsLauncher/actions/workflows/build.yml/badge.svg" alt="Build & Release"></a>
  <a href="https://github.com/RealmsLauncher/RealmsLauncher/releases"><img src="https://img.shields.io/github/v/release/RealmsLauncher/RealmsLauncher?display_name=tag&sort=semver" alt="Latest release"></a>
  <a href="https://github.com/RealmsLauncher/RealmsLauncher/blob/main/LICENSE"><img src="https://img.shields.io/badge/license-GPL--3.0-blue.svg" alt="GPL-3.0 License"></a>
  <a href="https://github.com/RealmsLauncher/RealmsLauncher/issues"><img src="https://img.shields.io/github/issues/RealmsLauncher/RealmsLauncher" alt="Issues"></a>
</p>

<p align="center">
  <a href="#features">Features</a> ·
  <a href="#download">Download</a> ·
  <a href="#building-from-source">Build</a> ·
  <a href="#contributing">Contributing</a> ·
  <a href="#support">Support</a>
</p>

> [!IMPORTANT]
> **Realms Launcher is an independent project and is not affiliated with, endorsed by, or sponsored by Mojang Studios, Microsoft, or Prism Launcher.**

## ✨ What is Realms Launcher?

Realms Launcher is a community-driven Minecraft launcher based on [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher). It keeps the powerful instance-management workflow people expect from Prism while adding Realms Launcher-specific changes, including support for offline accounts that do not use Microsoft authentication.

The goal is simple: give players and developers a clean, capable launcher that is easy to distribute, customize, and build from source.

## 🚀 Features

- **Offline accounts** — Create and use local/offline player identities without Microsoft authentication.
- **Microsoft accounts** — Official Microsoft/Mojang account authentication remains available when configured and supported.
- **Instance management** — Keep separate Minecraft installations, configurations, mods, resource packs, worlds, and settings organized.
- **Mod-loader friendly** — Designed around the same ecosystem of Minecraft instances and mod loaders supported by its Prism-based foundation.
- **Java management** — Includes the launcher's Java-runtime management and Java downloader support.
- **Cross-platform** — CI builds target Linux, Windows, and macOS, including ARM64/Apple Silicon where supported.
- **Open source** — The project is released under the GNU GPL v3.0 and can be inspected, modified, and built from source.
- **Automated releases** — GitHub Actions builds, tests, packages, and publishes platform artifacts.
- **Developer-friendly** — CMake presets, automated tests, clang-format/clang-tidy configuration, and repository contribution tooling are included.

## 📦 Download

The easiest way to get Realms Launcher is from the **[Releases](https://github.com/RealmsLauncher/RealmsLauncher/releases)** page.

| Platform | Architectures / builds |
| --- | --- |
| 🐧 Linux | x86_64, ARM64 |
| 🪟 Windows | x86_64, ARM64; MSVC and MinGW variants |
| 🍎 macOS | Intel (x86_64), Apple Silicon (arm64) |

> [!NOTE]
> Release availability depends on the CI jobs and artifacts produced for that release. If an artifact is missing, check the corresponding GitHub Actions run for the build status.

### Offline accounts & online servers

Offline accounts do **not** authenticate with Mojang/Microsoft services. They are intended for offline play and Minecraft servers that explicitly permit offline-mode players.

If a server requires an authenticated Minecraft account, use an appropriate Microsoft/Mojang account instead. Do not use offline accounts to impersonate another player or to bypass authentication controls.

## 🛠️ Building from source

Realms Launcher uses **CMake**, **Ninja**, **Qt 6**, and C++20. The repository includes CMake presets for the supported build targets.

For the full setup and platform-specific notes, see **[`docs/BUILDING.md`](docs/BUILDING.md)**.

### Quick build

After installing the required dependencies for your platform:

```bash
# Configure
cmake --preset linux

# Build
cmake --build --preset linux --config Release

# Test
ctest --preset linux --build-config Release
```

Use the matching preset on other platforms:

```text
macos
macos_arm64
macos_x86_64
macos_universal
windows_mingw
windows_msvc
linux
```

> [!TIP]
> Builds must use an out-of-source build directory. The project's CMake configuration intentionally rejects in-source builds.

## 🧪 Continuous integration

Every release build is designed to run through GitHub Actions. The current build matrix covers:

- Linux x86_64
- Linux ARM64
- Windows x86_64 with MSVC
- Windows x86_64 with MinGW/Clang64
- Windows ARM64 with MSVC
- Windows ARM64 with MinGW/ClangARM64
- macOS Apple Silicon
- macOS Intel

The workflow also runs the project's test suite before packaging. See [`.github/workflows/build.yml`](.github/workflows/build.yml) for the complete build and release pipeline.

## 🗂️ Project structure

```text
RealmsLauncher/
├── launcher/             # Main C++/Qt launcher application
├── libraries/            # Bundled/project libraries and integrations
├── cmake/                # CMake modules and build helpers
├── program_info/         # Application metadata, branding, and icons
├── tests/                # Automated tests
├── .github/
│   ├── actions/          # Reusable build/package actions
│   ├── workflows/        # CI, release, lint, and security workflows
│   └── ISSUE_TEMPLATE/   # Issue forms and project templates
├── CMakeLists.txt        # Main CMake configuration
├── CMakePresets.json     # Supported local build presets
├── CONTRIBUTING.md       # Contribution and coding guidelines
├── LICENSE               # GNU GPL v3.0
└── README.md             # You are here
```

## 🤝 Contributing

Contributions are welcome when they improve the project and follow its development guidelines.

Before opening a pull request:

1. Read **[`CONTRIBUTING.md`](CONTRIBUTING.md)**.
2. Keep changes focused and explain the problem they solve.
3. Run the relevant tests and formatting tools.
4. Review your changes for licensing and dependency implications.
5. Sign off your commits as required by the project's Developer Certificate of Origin.

### AI-assisted contributions

AI tools may be used as development aids, but contributors are expected to understand, review, and take responsibility for their changes. The repository's contribution policy contains the project's specific requirements for AI-assisted work and commit attribution.

## 🐛 Bug reports & feature requests

Before opening an issue, search existing issues to avoid duplicates.

- **Bug report:** [Open a bug report](https://github.com/RealmsLauncher/RealmsLauncher/issues/new?template=bug_report.yml)
- **Feature / suggestion:** [Open a suggestion](https://github.com/RealmsLauncher/RealmsLauncher/issues/new?template=suggestion.yml)
- **RFC / larger design proposal:** [Open an RFC](https://github.com/RealmsLauncher/RealmsLauncher/issues/new?template=rfc.yml)
- **All issues:** [Browse the issue tracker](https://github.com/RealmsLauncher/RealmsLauncher/issues)

When reporting a bug, include the launcher version, operating system, reproduction steps, relevant logs, and any useful build information. Never include account credentials, access tokens, or other secrets.

## 💬 Support

For project questions, troubleshooting, development discussion, and issue tracking, use the repository's **[Issues](https://github.com/RealmsLauncher/RealmsLauncher/issues)** and project documentation.

If you are reporting a reproducible bug, an issue is preferred over a private message so that the solution can benefit other users too.

## ⚖️ Legal & licensing

Realms Launcher is licensed under the **[GNU General Public License v3.0](LICENSE)**.

Realms Launcher is a derivative project based on [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher). The project retains applicable upstream notices and licensing requirements. See the repository source and license files for details.

Minecraft is a trademark of Mojang Studios. Microsoft and Mojang account services are trademarks and/or services of their respective owners. Realms Launcher is an independent community project.

## ❤️ Credits

Realms Launcher would not exist without the work of the open-source projects it builds upon.

Special thanks to:

- [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher) and its contributors for the launcher foundation.
- The Minecraft modding community and the many open-source projects that make the broader Minecraft ecosystem possible.
- Everyone who reports bugs, tests builds, submits patches, translates the launcher, or otherwise helps improve the project.

---

<p align="center">
  <sub>Built with ❤️ by the Realms Launcher community.</sub>
</p>
