# 🔥 FileForge: Your Minimalist File Alchemy Station 🧙‍♂️

**Description**

FileForge is not just another file conversion and compression tool; it's your personal alchemist's workshop for digital files. Crafted with the raw power of C++, Qt, libarchive, and FFmpeg, it transmutes your files with a single click. Imagine a tool that's as efficient as it is visually soothing, boasting an ultra-minimalist earthy aesthetic that blends dark grays, rich browns, and soft beiges. It’s like a serene forest clearing for your digital workflow.

**Purpose**

In a world overflowing with complex software, FileForge stands as a beacon of simplicity. It's designed to be your go-to, cross-platform solution for converting and compressing files without the headache. Whether you're archiving precious documents, transmuting audio tracks, or reshaping video files, FileForge handles it with elegance and speed.

**✨ Features: Where Simplicity Meets Power**

* **File Selection: 📂** Effortlessly choose any file from your system with a clean, intuitive dialog.
* **Format Flexibility: 🔄** Supports a versatile range of formats: `.txt` (copy), `.zip`, `.tar.gz`, `.mp3`, and `.mp4`.
* **Compression Mastery: 📦** Powered by libarchive, offering adjustable compression levels for `.zip` and `.tar.gz` archives.
* **Transcoding Wizardry: 🎬 🎵** Harnesses FFmpeg’s prowess for audio (`.mp3`) and video (`.mp4`) conversion, with bitrate control.
* **Zen UI: 🧘** A serene, earthy-toned interface with a single button for file selection, a format dropdown, a sleek progress bar, and intuitive Convert/Cancel buttons.
* **Real-Time Feedback: 📊** Accurate progress tracking keeps you informed every step of the way.
* **Instant Cancellation: 🛑** Halt any operation with a single click, ensuring you're always in control.
* **Robust Error Handling: 🚨** Clear, detailed error messages via popups, so you're never left in the dark.

**Code Architecture: A Peek Under the Hood 🛠️**

FileForge/
├── include/
│   ├── FileForgeWindow.hpp      # Elegant GUI declarations
│   ├── FileConverter.hpp        # Core conversion/compression logic
│   └── ConversionThread.hpp     # Asynchronous operation handling
├── src/
│   ├── main.cpp                 # The grand entrance
│   ├── FileForgeWindow.cpp      # GUI implementation
│   ├── FileConverter.cpp        # Conversion/compression implementation
│   └── ConversionThread.cpp     # Threading implementation
├── FileForge.pro                # Qt project file
└── README.md                    # This sacred scroll


* **FileForgeWindow:** The serene command center, managing the minimalist UI with Qt widgets.
* **FileConverter:** The heart of the alchemy, orchestrating compression (libarchive) and transcoding (FFmpeg).
* **ConversionThread:** The tireless worker, handling operations in a separate thread for smooth performance.

**Installation: Embark on Your Journey 🚀**

**Prerequisites**

* C++ Compiler: GCC, Clang, or MSVC (C++17 required)
* Qt: Version 5 or 6
* libarchive: For compression
* FFmpeg: For media transcoding

**Installation Steps (Ubuntu/Debian)**

```bash
sudo apt update
sudo apt install build-essential qt5-default libarchive-dev libavcodec-dev libavformat-dev libavutil-dev
For Other Platforms:

Windows: Qt Creator with MSVC or MinGW, libarchive and FFmpeg via vcpkg or manual builds.
macOS: Homebrew (brew install qt libarchive ffmpeg).
Build the Project

Bash

git clone <repository-url>
cd FileForge
qmake FileForge.pro
make
sudo make install # Optional
Running FileForge: Your First Transmutation 🔮

Launch: ./FileForge
Select File: Click "Select File" and choose your target.
Choose Format: Use the dropdown to select your desired output.
Convert: Click "Convert" and watch the magic happen.
Monitor: Track progress via the earthy brown progress bar.
Cancel: Click "Cancel" to halt the operation.
View Results: "Done" or "Failed" with detailed error messages.
Example Usage: Practical Alchemy 🧪

Compress document.txt to output.zip: Select, choose .zip, convert.
Transcode song.wav to output.mp3: Select, choose .mp3, convert.
Convert video.avi to output.mp4: Select, choose .mp4, convert.
Technical Insights: The Inner Workings ⚙️

Progress: Byte-accurate for compression, duration-based for transcoding.
Error Handling: Robust exception and error reporting.
Dependencies: Qt, libarchive, FFmpeg.
Future Visions: Expanding the Alchemy 🌌

Advanced transcoding options (bitrate, resolution).
Support for more formats (.rar, .flac).
File previews in the UI.
License: Open Source Magic 📜

This project is licensed under the MIT License. Modify and distribute freely!