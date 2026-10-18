# vst-LightWeaver
A C++20 / JUCE 8 VST3 plugin for Steinberg Cubase to control DMX light fixtures via MIDI CC and the arduino-MidiDmxBridge hardware.

### C++ Library development

The LightWeaver VST plugin is developed in C++ and uses CMake for configuration. The library is tested using gtest and documented using Doxygen.

```bash DOS
mkdir build
cmake -S . -B ./build
cmake --build ./build
ctest --test-dir ./build/
```

## Local Cubase 14 Deployment (Windows)

For local development, a symbolic link in the system-wide VST3 folder avoids copying the plugin after every build. Build the configuration you want to test:

```powershell
cmake --build build --config Release
```

The VST3 bundle is written to `build\LightWeaver_artefacts\Release\VST3\LightWeaver.vst3`. Cubase 14 has been tested successfully with a symbolic link directly to the plugin binary at `Contents\x86_64-win\LightWeaver.vst3`, so the system-wide VST3 folder does not need a copy of the complete bundle. For a Debug build, replace `Release` with `Debug` in both the build command and target path.

Create the link once from an elevated PowerShell session (or with Windows Developer Mode enabled):

```powershell
New-Item -ItemType SymbolicLink `
	-Path 'C:\Program Files\Common Files\VST3\LightWeaver.vst3' `
	-Target 'G:\Software\vst-LightWeaver\build\LightWeaver_artefacts\Release\VST3\LightWeaver.vst3\Contents\x86_64-win\LightWeaver.vst3'
```

If a link already exists at that path, close Cubase and remove only the existing link before recreating it. Verify that it is a link first; do not remove a real plugin directory. After building, restart Cubase or rescan plug-ins in the Plug-in Manager so the host loads the updated binary. Keep Cubase closed while rebuilding if the plugin binary is in use.

The software documentation can then be accessed at [./build/html/index.html](./build/html/index.html), the code coverage results at [./build/coverage/index.html](./build/coverage/index.html).

## How to Run Codespell

To run [codespell](https://github.com/codespell-project/codespell), you need to have Python 3 installed on your system. You can install Python 3 from the official website: https://www.python.org/downloads/.

Once you have Python 3 installed, you can install [codespell](https://github.com/codespell-project/codespell) using pip, the Python package manager. Open a terminal or command prompt and type the following command:

```bash
pip install codespell
```

This will install [codespell](https://github.com/codespell-project/codespell) and all its dependencies.

To run [codespell](https://github.com/codespell-project/codespell), navigate to the root directory of your project in the terminal or command prompt and type the following command:

```bash
codespell
```

This will run [codespell](https://github.com/codespell-project/codespell) on all the files in the current directory and its subdirectories. [codespell](https://github.com/codespell-project/codespell) will check for common misspellings and suggest corrections. You can use the `-w` option to write the changes directly to the files.

```bash
codespell -w
```

For more information on how to use [codespell](https://github.com/codespell-project/codespell), including a list of all the available options, please refer to the official documentation: https://github.com/codespell-project/codespell#usage.
