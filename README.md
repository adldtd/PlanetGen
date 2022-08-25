# PlanetGen

### The current GUI
![GUI](/screenshots/WorldGenerator.png)

A C++ GUI kit which uses Perlin Noise to create Earth-like worlds. Comes with customization settings to modify world size, land, climate, and moisture. This project uses SFML (https://www.sfml-dev.org/), the TGUI package (https://tgui.eu/),
libpng (+ zlib), and a Perlin Noise library(https://github.com/Reputeless/PerlinNoise).

## Installation (Visual Studio; on Windows)

To clone the project, run `git clone https://github.com/adldtd/PlanetGen`

Make sure you have C++ 17 installed. You will need to install both SFML and TGUI for the project to compile. For SFML, the version is "Visual C++ 14 (2015) - 32-bit", and for TGUI, the version is "0.9.5 Visual Studio (2017-2022) - 32bit". Then, follow the tutorials on each site to link the libraries to the project.

## Installing libpng

First, go to https://sourceforge.net/projects/libpng/files/. Click on "libpng16," then download libpng 1.6.37. Head back to the files section, and then install zlib version 1.2.11.

After extracting both folders, take out the "zlib1211" folder out of the zlib installation, and place "lpng1637" alongside that folder in a common directory. Then, rename "zlib1211" to simply "zlib".

After that, in the "lpng" folder, go to "projects/visualc71". Open libpng.sln with Visual Studio. You should get a prompt asking you to upgrade certain files; click OK to all. From there, you can compile and build all packages, either in debugging or release mode.

After building the project, in the visualc71 folder, there should be folders with "DLL" in their names. This is where the created LIB and DLL files for both libpng and zlib are located. In the WorldGen project, link the libraries as you did with SFML and TGUI (header locations go in Additional Include Directories, .LIB locations go in Additional Library Directories, .LIB files go in Additional Dependencies.) 

NOTE: If you get an error stating that a .DLL file is unavailable, simply go into the "Debug" folder of your VS project, and copy and paste that file into there.