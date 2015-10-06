# sdl2-tetris

---

## About

Simple clone of Tetris in C/C++ using SDL2, for educational purposes.

##Dependencies

This project depends on [SDL2](https://www.libsdl.org) and [SDL_ttf 2.0](https://www.libsdl.org/projects/SDL_ttf)

## Windows

The [SDL2 Development libraries for Visual C++](https://www.libsdl.org/download-2.0.php) with Visual Studio 2015 versions of SDLmain.lib are checked in to the `3rdparty` folder for convenience.

## Linux

Install using your package manager, e.g.

	$ sudo apt-get install libsdl2-dev
	$ sudo apt-get install libsdl2-ttf-dev
	
or [download](https://www.libsdl.org/download-2.0.php) and install from source

### Raspberry Pi

The SDL2 packages (libsdl2-dev, libsdl2-ttf-dev) are available to install in Raspbian Jessie. On Wheezy and earlier you'll have to install from source.

## Building

This project uses [GENie](https://github.com/bkaradzic/genie) to generate the build files for the target platform.

###Linux

	$ git clone https://github.com/howprice/sdl2-tetris
	$ cd sdl2-tetris
	$ tools/bin/linux/genie gmake
	$ cd build
	$ make

n.b. You may need to chmod+x genie executable - I haven't figured out how to make the checked in file executable.

`make` with no confic specified defaults to the debug config for native architecture (usually 64-bit). Can call `make config=<xxx>` where `xxx` can be `debug`, `release`, `debug32`, `release32`, `debug64` or `release64`. 
	
### Raspberry Pi

Use `tools/bin/raspberry-pi/genie`
	
###Windows

	$ git clone https://github.com/howprice/sdl2-tetris
	$ tools\bin\windows\genie.exe vs2015   -- You can also use vs2013 etc.
	
Open build/hello-sdl2.sln with Visual Studio 2015. 

The [SDL2 Development libraries for Visual C++](https://www.libsdl.org/download-2.0.php) are checked in to 3rdparty for convenience. I have overwritten the pre-Visual Studio 2015 SDLmain.lib files with those built for VS2015 for convenience. If you want to use 2013 or earlier then download the zip and overwrite. n.b. SDLmain.lib is built against the CRT (which was re-written for VS2015) but SDL.lib does not use the CRT. Ask Google for "SDL2 Visual Studio 2015" for more info.

## Cleaning the build files

###Linux

	$ tools/bin/linux/genie clean
	
###Raspberry Pi

	$ tools/bin/raspberry-pi/genie clean

###Windows

	$ tools/bin/windows/genie clean

## Running

### Windows

Genie is configured to generate `sdl2-tetris.vcxproj.user` which correctly sets the debugger working directory to `data/`

### Linux

	$ cd data
	$ ../build/sdl2-tetris

## Thanks

Classic Console font from http://webdraft.hu/fonts/classic-console
