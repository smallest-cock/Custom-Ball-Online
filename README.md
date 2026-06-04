# Custom Ball Online (BakkesMod plugin)
Enable custom ball skins in online Rocket League matches

<img src="assets/images/plugin_screenshot.png" width="70%">

## 🔧 Installation
See the [latest release](https://github.com/smallest-cock/Custom-Ball-Online/releases/latest) and follow the installation steps

## 📖 Usage
1. Install the plugin
    - Extract `CustomBallOnline.zip` and run `install.bat`
  
2. Install your ball skins in the plugin's `BallTextures` folder:
    ```
    %LOCALAPPDATA%\bakkesmod\bakkesmod\data\CustomBallOnline\BallTextures
    ```
    - Check out [this tutorial](https://youtu.be/MMai_foKfyo) to learn how to install skins

3. Select a ball skin in the plugin menu

4. Join a match, and your skin should automatically be applied 🥳

## 💻 Console Commands
The following commands aren't necessary for normal usage, but they exist if you ever need them. You can enter them in the BakkesMod console (`F6`) or bind them to keys:

| Command | Description |
|---------|-------------|
| `cbo_apply_ball_skin` | Apply selected ball skin |
| `cbo_clear_saved_textures` | Clear all cached ball textures |
| `cbo_clear_unused_saved_textures` | Clear all unused cached ball textures |

<br>

>[!TIP]
>If you're experiencing issues with skins being applied automatically, bind `cbo_apply_ball_skin` to a key to apply them whenever you want


## 🔨 Building
> [!NOTE]  
> Building this plugin requires the **MSVC** toolchain, due to reliance on the Windows SDK and the need for ABI compatibility with Rocket League

### 1. Initialize submodules
After cloning the repo, run this helper script to optimally initialize submodules:
```
./scripts/init-submodules.bat
```

<details> <summary>🔍 Why this instead of <code>git submodule update --init --recursive</code> ?</summary>
   <ul>
       <li>Avoids downloading 200MB of history for the <strong>nlohmann/json</strong> library</li>
       <li>Ensures Git can detect future updates for the other submodules</li>
   </ul>
</details>

### 2. Build with CMake
> [!NOTE]
> Before building on Windows, the MSVC environment **must** be initialized.
> This is normally handled automatically by IDEs or certain editor extensions like [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools), but if you're building from the command line, use one of the following methods:
>
> - Use an appropriate Windows terminal profile:
>    - `Developer PowerShell for VS 2022`
>    - `Developer Command Prompt for VS 2022`
> - Or run this script once per shell session:
>   ```
>   C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat
>   ```

1. Install [CMake](https://cmake.org/download) and [Ninja](https://github.com/ninja-build/ninja/releases)
   - If you prefer a build system other than Ninja, just create a `CMakeUserPresets.json` and specify it there. [More info here](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
2. Run this to configure (will generate build files in `./build`):
    ```
    cmake --preset ninja-release
    ```
3. Run this to build:
    ```
    cmake --build --preset Ninja-Release
    ```
   - The built binaries will be in `./plugins`

## 🚧 Errors / Troubleshooting
Join [the discord](https://discord.gg/tHZFsMsvDU) if you have questions, errors, or suggestions 🐖

## ❤️ Support
If you found this plugin helpful and would like to donate:

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/sslowdev)
