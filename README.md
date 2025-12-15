# Custom Ball Online (BakkesMod plugin)
Enable custom ball skins in online Rocket League matches

<img src="assets/images/plugin_screenshot.png" width="70%">

## 🔧 Installation
See the [latest release](https://github.com/smallest-cock/Custom-Ball-Online/releases/latest) and follow the installation steps

## 📖 Usage
1. Install the correct version of the plugin
    - Install `CustomBallOnline-AC.zip` if you use AlphaConsole, otherwise use the superior `CustomBallOnline.zip`
  
2. Install your ball skins in the correct `BallTextures` folder:
    | Plugin version | `BallTextures` path |
    |----------------|:-----------------:|
    | Regular | `bakkesmod\data\CustomBallOnline\BallTextures` |
    | AlphaConsole | `bakkesmod\data\acplugin\BallTextures` |
    - Check out [this tutorial](https://youtu.be/MMai_foKfyo) to learn how to install ball skins

4. Select a skin in the plugin menu. If you're using AlphaConsole, this means selecting a skin in the AlphaConsole menu.

5. Join a match, and your skin should automatically be applied 🥳

## 💻 Console Commands
The following commands aren't necessary for regular usage, but they exist if you ever need them. You can enter them in the BakkesMod console (`F6`) or bind them to keys:

| Command | Description |
|---------|-------------|
| `cbo_apply_ball_skin` | Apply selected ball skin |
| `cbo_clear_saved_textures` | Clear all cached ball textures |
| `cbo_clear_unused_saved_textures` | Clear all unused cached ball textures |

<br>

>[!TIP]
>If you're experiencing issues with skins being applied automatically, bind `cbo_apply_ball_texture` to a key to apply them whenever you want


## 🔨 Building
> [!NOTE]  
> Building this plugin requires **64-bit Windows** and the **MSVC** toolchain, due to reliance on the Windows SDK and the need for ABI compatibility with Rocket League

### 1. Initialize submodules
Initialize the submodules after cloning the repo:

Run `./scripts/init-submodules.bat` to initialize the submodules optimally

<details> <summary>🔍 Why this instead of <code>git submodule update --init --recursive</code> ?</summary>
   <ul>
       <li>Avoids downloading 200MB of history for the <strong>nlohmann/json</strong> library</li>
       <li>Ensures Git can detect future updates for the other submodules</li>
   </ul>
</details>

### 2. Build with CMake
1. Install [CMake](https://cmake.org/download) and [Ninja](https://github.com/ninja-build/ninja/releases)
   - If you prefer a build system other than Ninja, just create a `CMakeUserPresets.json` and specify it there. [More info here](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
2. Run this to configure (will generate build files in `./build`):
    ```
    cmake --preset windows-x64-msvc
    ```
3. Run this to build:
    ```
    cmake --build ./build
    ```
   - The built binaries will be in `./plugins`

## 🚧 Errors / Troubleshooting
Join [the discord](https://discord.gg/tHZFsMsvDU) if you have questions, errors, or suggestions 🐖

## ❤️ Support
If you found this plugin helpful and would like to donate:

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/sslowdev)
