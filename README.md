# hello-sdl2

---

## About

Test repository to investigate integrating an external library (SDL2) with a GitHub repository

##Dependencies

This project depends on [SDL2](https://www.libsdl.org)

## Linux
	$ apt-get install libsdl2-dev

## Windows

The [SDL2 Development libraries for Visual C++](https://www.libsdl.org/download-2.0.php) with Visual Studio 2015 versions of SDLmain.lib are checked in to the `3rdparty` folder for convenience.

## Building

This project uses [GENie](https://github.com/bkaradzic/genie) to generate the build files for the target platform.

###Linux

	$ git clone https://github.com/howprice/hello-sdl2
	$ tools/bin/linux/genie gmake
	$ cd build
	$ make

`make` with no confic specified defaults to the debug config for native architecture (usually 64-bit). Can call `make config=<xxx>` where `xxx` can be `debug`, `release`, `debug32`, `release32`, `debug64` or `release64`. 
	
###Windows

	$ git clone https://github.com/howprice/hello-sdl2
	$ tools\bin\windows\genie.exe vs2015   -- You can also use vs2013 etc.
	
Open build/hello-sdl2.sln with Visual Studio 2015. 

The [SDL2 Development libraries for Visual C++](https://www.libsdl.org/download-2.0.php) are checked in to 3rdparty for convenience. I have overwritten the pre-Visual Studio 2015 SDLmain.lib files with those built for VS2015 for convenience. If you want to use 2013 or earlier then download the zip and overwrite. n.b. SDLmain.lib is built against the CRT (which was re-written for VS2015) but SDL.lib does not use the CRT. Ask Google for "SDL2 Visual Studio 2015" for more info.

## Cleaning the build files

###Linux

	$ tools/bin/linux/genie clean

###Windows

	$ tools/bin/windows/genie clean
