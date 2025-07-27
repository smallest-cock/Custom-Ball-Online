# Custom Ball Online (BakkesMod plugin)
Enables custom ball skins in online matches

<img src="assets/images/plugin_screenshot.png" width="80%">

## 🎮 How to use
1. Have the plugin installed and a ball skin selected
   
2. Join a match, and your ball skin should automatically be applied

<br>

>[!TIP]
>Bind the `cbo_apply_ball_texture` command to apply the ball skin whenever you want
>
>(like if the skin wasn't applied automatically for some reason)

## 🔧 How to install
Follow the steps in [Releases](https://github.com/smallest-cock/Custom-Ball-Online/releases/latest)

## 🔨 Building
> [!NOTE]  
> Building this plugin requires **64-bit Windows** and the **MSVC** toolchain
> - Due to reliance on the Windows SDK and the need for ABI compatibility with Rocket League

### 1. Initialize submodules
Initialize the submodules after cloning the repo:

Run `./scripts/init-submodules.bat` to initialize the submodules in an optimal way

<details> <summary>🔍 Why this instead of <code>git submodule update --init --recursive</code> ?</summary>
<li>Avoids downloading 200MB of history for the <strong>nlohmann/json</strong> library</li>
<li>Ensures Git can detect future updates for the other submodules</li>
</details>

### 2. Build with CMake
1. Install [CMake](https://cmake.org/download) and [Ninja](https://github.com/ninja-build/ninja/releases) (or any other build system you prefer)
2. Run `cmake --preset windows-x64-msvc` (or create your own preset in a `CMakeUserPresets.json`) to generate build files in `./build`
3. Run `cmake --build build`
   - The built binaries will be in `./plugins`

## 🚧 Errors / Troubleshooting
Join [the discord](https://discord.gg/tHZFsMsvDU) if you have questions, errors, or need help

## ❤️ Support
If you found this plugin helpful and would like to donate:

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/sslowdev)