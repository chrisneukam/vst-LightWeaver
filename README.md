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
