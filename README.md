# Audio Spectrogram Visualization

![Spectrogram Visualization](screenshot.png)

## Overview

This cross-platform software captures audio from the user's microphone and generates a Spectrogram Visualization in real-time. The project is implemented in C++ using the Qt framework.

## Features

- Real-time audio capture
- Spectrogram Visualization
- Cross-platform (Windows, macOS, Linux)

## Prerequisites

Before running the application, make sure you have the following dependencies installed:

- [Qt](https://www.qt.io/download) (version 6.6.1)

## Building the Project

Clone this repository:

```bash
git clone git@github.com:msbeigiai/asrv.git
cd asrv
qmake
make      # or 'nmake' on Windows
```

## Running the Application

On Windows:

```bash
./asrv.exe
```

On macOS:

```bash
./asrv.app/Contents/MacOS/asrv
```

On Linux:

```bash
./asrv
```

## Usage

- Upon launching the application, it will start capturing audio from the default microphone.

- The Spectrogram Visualization will be displayed in the graphical user interface.

## Configuration

- Not configuration needed to run the project

## Contributing

Feel free to contribute to the project. Follow the contribution guidelines for details.

## License

This project is licensed under the MIT License.

## Support

For any issues or questions, please open an issue.

Remember to create the `CONTRIBUTING.md` and `LICENSE` files mentioned in the template. Ensure that the Qt version specified in the prerequisites matches the version you used for development.

This README template includes common sections found in well-documented projects and should serve as a good starting point for your documentation.
