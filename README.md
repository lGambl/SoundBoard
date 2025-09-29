# Soundboard

`Soundboard` is a desktop soundboard built with Qt that lets you trigger audio clips from a list or via global hotkeys. Each clip can be given a friendly display name, played or stopped on demand, and assigned to a keyboard shortcut so you can fire sounds even while the app runs in the background.

## Features
- Play multiple audio clips with per-item widgets and a shared volume slider.
- Assign global hotkeys (powered by QHotkey) to play clips or stop them all at once.
- Persist your library of clips in `items.txt`, including display names and key bindings.
- Choose the audio output device to route playback to the right speakers.

## Prerequisites
- CMake 3.16+
- A C++17 compiler (MSVC, Clang, or GCC)
- Qt 6 (Widgets + Multimedia modules) or Qt 5 with the same modules
- Git (to fetch the bundled QHotkey dependency)

**Tip:** Clone the repository with submodules to pull in QHotkey automatically.
```
git clone --recurse-submodules <repo-url>
```

## Building
The project uses the standard CMake workflow. Replace `<qt-prefix>` with the path where Qt installs its CMake files (for example `C:/Qt/6.7.2/msvc2019_64/lib/cmake` on Windows or `/opt/Qt/6.7.2/gcc_64/lib/cmake` on Linux).

### Windows (MSVC)
```powershell
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="<qt-prefix>"
cmake --build build
```
You can swap `Ninja` for `"Visual Studio 17 2022"` if you prefer a Visual Studio solution.

### macOS / Linux
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="<qt-prefix>"
cmake --build build
```

Qt Creator users can open the `CMakeLists.txt` directly; the IDE will configure and build the project automatically, reusing the same CMake settings. The CMake target is named `Soundboard`, so every generator produces a matching executable or bundle.

## Running
After a successful build, the executable lives in the build directory:
- Windows: `build/Soundboard.exe`
- macOS: `build/Soundboard.app`
- Linux: `build/Soundboard`

Launch the binary and keep `items.txt` in the project root (one level above the build folder) so the app can load your saved clips. Each line in `items.txt` uses the format:
```
"<absolute-or-relative-audio-path>" <status-flag> "<display-name>" <key-code>
```
The status flag is currently `0` for normal items, and the key code stores the Qt key value for the clip's hotkey (0 means no binding).

## Usage
1. Click **Add** to pick an audio file and give it a display name.
2. Use the play/stop controls beside each item to audition clips.
3. Press **Set Key** on an item to capture a global hotkey; press **Set Stop-All Key** to define the emergency mute shortcut.
4. Adjust the overall volume slider or select a different output device from the **Devices** menu.

Hotkeys work system-wide, so you can trigger sounds while another application has focus. Use the stop-all shortcut to immediately cut every playing clip.

## Troubleshooting
- If the app starts empty, confirm that `items.txt` exists in the project root (the folder containing `CMakeLists.txt`).
- When CMake cannot find Qt, double-check the `CMAKE_PREFIX_PATH` value or source the Qt environment script (`qtvars.bat` or `qt-env.sh`).
- Global hotkeys require the QHotkey module; ensure the `external/QHotkey` folder is present after cloning.
