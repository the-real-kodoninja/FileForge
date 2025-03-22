FileForge
Description
FileForge is a lightweight, minimalistic file conversion and compression tool built in C++ using Qt, libarchive, and FFmpeg. Designed with simplicity and efficiency in mind, it allows users to select a file, choose an output format, and convert or compress it with real-time progress tracking. The interface adopts an ultra-minimal earthy aesthetic (dark grays, browns, and beiges), making it both functional and visually appealing.

Purpose
The purpose of FileForge is to provide a straightforward, cross-platform solution for converting and compressing files without overwhelming users with complex options. Whether you need to archive a document into a .zip or .tar.gz, transcode audio to .mp3, or convert video to .mp4, FileForge handles it with a single click, offering cancellation and progress feedback along the way.

Features
File Selection: Pick any file via a simple dialog.
Output Formats: Supports .txt (copy), .zip, .tar.gz, .mp3, and .mp4.
Compression: Uses libarchive for .zip and .tar.gz with adjustable compression levels.
Transcoding: Leverages FFmpeg for audio (.mp3) and video (.mp4) conversion with basic bitrate settings.
Minimal UI: Earth-toned design with a single button for file selection, a format dropdown, a progress bar, and Convert/Cancel buttons.
Real-Time Progress: Displays accurate progress for compression and transcoding.
Cancellation: Stop ongoing operations with a Cancel button.
Error Handling: Detailed error messages via popups when operations fail.
Code Structure
text

Collapse

Wrap

Copy
FileForge/
├── include/
│   ├── FileForgeWindow.hpp      # GUI declaration
│   ├── FileConverter.hpp        # Conversion/compression logic
│   └── ConversionThread.hpp     # Threading for async operations
├── src/
│   ├── main.cpp                 # Application entry point
│   ├── FileForgeWindow.cpp      # GUI implementation
│   ├── FileConverter.cpp        # Conversion/compression implementation
│   └── ConversionThread.cpp     # Threading implementation
├── FileForge.pro                # Qt project file
└── README.md                    # This file
FileForgeWindow: Manages the minimal UI with Qt widgets and handles user interactions.
FileConverter: Core logic for compression (libarchive) and transcoding (FFmpeg).
ConversionThread: Runs operations in a separate thread, providing progress updates and cancellation.
Installation
Prerequisites
C++ Compiler: GCC, Clang, or MSVC (C++17 support required).
Qt: Version 5 or 6 (e.g., qt5-default or qt6-base).
libarchive: For compression (e.g., libarchive-dev).
FFmpeg: Libraries for media transcoding (libavcodec-dev, libavformat-dev, libavutil-dev).
Install Packages (Ubuntu/Debian Example)
bash

Collapse

Wrap

Copy
sudo apt update
sudo apt install build-essential qt5-default libarchive-dev libavcodec-dev libavformat-dev libavutil-dev
For Other Platforms:
Windows: Use Qt Creator with MSVC or MinGW, install libarchive and FFmpeg via vcpkg or manual builds.
macOS: Use Homebrew (brew install qt libarchive ffmpeg).
Build the Project
Clone or download the repository:
bash

Collapse

Wrap

Copy
git clone <repository-url>
cd FileForge
Generate build files with qmake:
bash

Collapse

Wrap

Copy
qmake FileForge.pro
Compile and link:
bash

Collapse

Wrap

Copy
make
(Optional) Install:
bash

Collapse

Wrap

Copy
sudo make install  # Installs to /usr/local/bin (Linux)
Running FileForge
Step-by-Step Guide
Launch the Application:
bash

Collapse

Wrap

Copy
./FileForge
A small window (400x150) opens with a dark gray background and earthy tones.
Select a File:
Click "Select File".
Choose any file (e.g., document.txt, song.wav, video.mp4).
The filename appears in the status label.
Choose an Output Format:
Use the dropdown to select .txt, .zip, .tar.gz, .mp3, or .mp4.
The output will be saved as output.<format> in the input file’s directory.
Start Conversion:
Click "Convert".
The status changes to "Processing...", and the progress bar fills as the operation runs.
Monitor Progress:
Watch the earthy brown progress bar update in real time.
Cancel (Optional):
Click "Cancel" to stop the operation; the status resets to "Cancelled".
View Results:
On success: Status shows "Done: <size> bytes".
On failure: Status shows "Failed", and an error popup provides details.
Example Usage
Compress document.txt to output.zip:
Select document.txt, choose .zip, click "Convert".
Transcode song.wav to output.mp3:
Select song.wav, choose .mp3, click "Convert".
Convert video.avi to output.mp4:
Select video.avi, choose .mp4, click "Convert".
How FileForge Works
Architecture
GUI Layer (FileForgeWindow):
Built with Qt, it uses a QVBoxLayout with minimal widgets: a file button, format combo, progress bar, status label, and two buttons (Convert/Cancel).
Styled with earthy tones (e.g., #2E2E2E background, #8B5A2B progress).
Conversion Logic (FileConverter):
Compression: Uses libarchive to create .zip or .tar.gz archives. It reads the input file in chunks, writes to the archive, and tracks progress via bytes processed.
Transcoding: Uses FFmpeg’s C API to decode and re-encode media files. It supports .mp3 (audio) and .mp4 (H.264 video) with a fixed bitrate (e.g., 128 kbps for audio).
Threading (ConversionThread):
Runs conversion in a QThread to keep the UI responsive.
Emits progressUpdated signals for real-time feedback and conversionFinished with results or errors.
Supports cancellation via a cancelled flag.
Workflow
User Input: User selects a file and format.
Thread Launch: Clicking "Convert" spawns a ConversionThread.
Processing:
For .zip/.tar.gz: libarchive compresses the file, reporting progress as bytes are written.
For .mp3/.mp4: FFmpeg decodes the input, re-encodes to the target format, and estimates progress based on duration or samples.
For .txt: Simply copies the file.
Feedback: Progress updates the UI’s QProgressBar; completion updates the status label.
Cancellation: Clicking "Cancel" stops the thread, resetting the UI.
Technical Notes
Progress: Accurate for compression (byte-based); approximate for transcoding (duration-based).
Error Handling: Exceptions and FFmpeg/libarchive errors are caught and displayed via QMessageBox.
Dependencies: Qt for GUI, libarchive for archives, FFmpeg for media.
Future Enhancements
Add transcoding options (bitrate, resolution) via a settings dialog.
Support more formats (e.g., .rar, .flac).
Include file previews in the UI.
License
This project is open-source under the MIT License. Feel free to modify and distribute!

This README.md provides a comprehensive guide to FileForge. Save it in the project root as README.md. Let me know if you’d like to tweak anything or add more details! What’s next on your mind?