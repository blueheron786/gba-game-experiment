# GBA Project

## Overview
This project is a simple Game Boy Advance (GBA) application that demonstrates basic graphics rendering. It sets the video mode to bitmap and displays the text "Hello World" on the screen.

## Project Structure
```
gba-project
├── src
│   └── main.cpp        # Main entry point of the GBA application
├── include
│   └── gba.h          # Header file for GBA-specific functions and constants
├── Makefile            # Build instructions for the project
└── README.md           # Documentation for the project
```

## Setup Instructions
1. Ensure you have a GBA development environment set up, including the necessary tools for compiling and running GBA applications.
2. Clone this repository or download the project files to your local machine.

## Building the Project
To build the project, navigate to the project directory in your terminal and run the following command:
```
make
```
This will compile the source files and generate the executable.

## Usage
- Grab a copy of `gbafix.exe` and drop it into your repo directory
- Run `make` (and on subsequent builds, I recommend `make clean`)
  
After building the project, you can run the generated executable on a GBA emulator like mGBA or flash it and play it on hardware.

## Contributing
Feel free to contribute to this project by submitting issues or pull requests.
