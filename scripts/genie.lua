--[[ 

TODO:
	
	* clean action should remove the build directory even if it is not empty
--]] 

solution "sdl2-tetris"
	location "../build"
	configurations { "Debug", "Release" }
	platforms { "native", "x32", "x64" }

	project "sdl2-tetris"
		location "../build"
		kind "ConsoleApp"
		language "C++"
		files { "../src/**.h", "../src/**.cpp" }
		flags { "ExtraWarnings", "FatalWarnings" }
		links { "SDL2", "SDL2_ttf" }
		targetdir "../bin"
		debugdir "../data"		-- debugger working directory
		
		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" }

		configuration "windows"
			includedirs { "../3rdparty/SDL2-2.0.3/include" }
			includedirs { "../3rdparty/SDL2_ttf-2.0.12/include" }
			links { "SDL2main" }
			
			-- There is a single SDLmain.lib that is built against the Multi-threaded DLL (/MD) i.e. there is no debug lib built against Multi-threaded Debug DLL (/MD
			flags { "ReleaseRuntime" }  
			
			-- Disable compiler warnings. These end up in the Project Settings -> C/C++ -> Command Line -> Additional Options, rather than C/C++ -> Advanced -> Disable Specific Warnings 
			buildoptions { "/wd4127" } -- conditional expression is constant
			buildoptions { "/wd4505" } -- unreferenced local function has been removed
			
		configuration { "windows", "not x64" }
			libdirs { "../3rdparty/SDL2-2.0.3/lib/x86" }
			libdirs { "../3rdparty/SDL2_ttf-2.0.12/lib/x86" }
			postbuildcommands { 
				"copy ..\\3rdparty\\SDL2-2.0.3\\lib\\x86\\*.dll ..\\bin",
				"copy ..\\3rdparty\\SDL2_ttf-2.0.12\\lib\\x86\\*.dll ..\\bin" }

		configuration { "windows", "x64" }		
			libdirs { "../3rdparty/SDL2-2.0.3/lib/x64" }
			libdirs { "../3rdparty/SDL2_ttf-2.0.12/lib/x64" }
			postbuildcommands { 
				"copy ..\\3rdparty\\SDL2-2.0.3\\lib\\x64\\*.dll ..\\bin", 
				"copy ..\\3rdparty\\SDL2_ttf-2.0.12\\lib\\x64\\*.dll ..\\bin" }

		configuration "linux"
			
			buildoptions { "-std=c++11" }
			buildoptions { "-Wno-switch" }
			buildoptions { "-Wno-unused-function" }
			buildoptions { "-Wno-missing-field-initializers" }
			
--			buildoptions { "`sdl2-config --cflags`" }  -- magic quotes are shell-dependent
			buildoptions { os.outputof("sdl2-config --cflags") }  -- requires GENie to be run on target machine

--			linkoptions { "`sdl2-config --libs`" } -- magic quotes are shell-dependent
			linkoptions { os.outputof("sdl2-config --libs") } -- requires GENie to be run on target machine

if _ACTION == "clean" then
	os.rmdir("../bin")
	os.rmdir("../build/.vs")		-- this doesn't seem to work because the file is write protected
	os.rmdir("../build")			-- this doesn't work because the directory contains .vs folder
end
