# GD UI

A native Geometry Dash mod foundation using **Geode 5.10.1**, **GD 2.2081**,
and **C++23**. Mod ID: `souplyy.gd-ui`.

The first interface component is a full-width charcoal top bar on the main menu.
Its custom pale gear at the top-left opens Geometry Dash settings. The enable setting takes effect after a
restart. The full interface redesign is future work.

## Local setup

Install Geometry Dash and [Geode](https://geode-sdk.org/install/), a C++23-capable
compiler, CMake 3.25 or newer, and the [Geode CLI](https://docs.geode-sdk.org/getting-started/geode-cli/).
On macOS, install the Xcode command line tools and use:

```sh
brew install cmake geode-sdk/geode/geode-cli
geode sdk install
geode sdk update 5.10.1
geode sdk install-binaries --version 5.10.1
```

Restart your terminal and confirm `GEODE_SDK` points to the installed SDK.
Keep the SDK and its binaries on the same version as `mod.json`.

## Build locally

From this repository:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DGEODE_DONT_INSTALL_MODS=ON
cmake --build build --config RelWithDebInfo --parallel
```

The package is `build/souplyy.gd-ui.geode`. Install it using Geode's local package
import, or copy it to your game's `geode/mods` directory while the game is closed.
Install **Node IDs 1.23.3 or newer** through Geode as well.

For automatic installation while developing, run `geode config setup` to select
your GD installation, then build with `geode build`. Unlike the CMake commands
above, a configured CLI profile can automatically install the resulting mod.

For Android, install the Android NDK and matching Geode binaries, then use
`geode build -p android64` or `geode build -p android32`. For iOS, use a Mac with
the full Xcode iPhone SDK and `geode build -p ios`.

## Source structure

| File | Purpose |
| --- | --- |
| `mod.json` | Identity, supported versions, dependencies, settings |
| `src/main.cpp` | Mod-loaded entry point and logging |
| `src/hooks/MenuLayer.cpp` | Calls original menu initialization, adds the top bar |
| `src/ui/TopBar.*` | Dark top bar, custom gear, and settings control |
| `CMakeLists.txt` | C++23 library and Geode package generation |
| `.github/workflows/build.yml` | Builds for Windows, macOS, Android, and iOS |

Give custom nodes namespaced IDs with `_spr`. Find existing controls through
members or string IDs, check unknown node types with `typeinfo_cast`, and call
`updateLayout()` after adding controls to a shared menu. Keep GD/Cocos UI work on
the main thread. Use `Fields` for per-instance data in hooks and `Ref`/`WeakRef`
when pointers need to outlive the immediate call.

## Verification

A successful compilation does not establish in-game compatibility. Test these
flows on the target platform before release:

- Start GD and open/close settings using the top-left gear and Escape/back.
- Leave the main menu and return; check that only one top bar appears.
- Disable the setting, restart, and confirm the top bar is hidden; re-enable and restart.
- Check smaller aspect ratios and coexistence with other menu mods.

The GitHub workflow uploads build artifacts; it does not publish releases or
submit anything to the Geode index. This is a local native mod, with no website
or hosting service.

See [Geode reference notes](docs/geode-reference.md) for the documentation baseline.
