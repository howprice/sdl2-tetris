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
		links { "SDL2" }
		targetdir "../bin"
		
		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" }

		configuration "windows"
			includedirs { "../3rdparty/SDL2-2.0.3/include" }
			links { "SDL2main" }
			
			-- There is a single SDLmain.lib that is built against the Multi-threaded DLL (/MD) i.e. there is no debug lib built against Multi-threaded Debug DLL (/MD
			flags { "ReleaseRuntime" }  
			
		configuration { "windows", "not x64" }
			libdirs { "../3rdparty/SDL2-2.0.3/lib/x86" }
			postbuildcommands { "copy ..\\3rdparty\\SDL2-2.0.3\\lib\\x86\\SDL2.dll ..\\bin\\SDL2.dll" }

		configuration { "windows", "x64" }		
			libdirs { "../3rdparty/SDL2-2.0.3/lib/x64" }
			postbuildcommands { "copy ..\\3rdparty\\SDL2-2.0.3\\lib\\x64\\SDL2.dll ..\\bin\\SDL2.dll" }

		configuration "linux"
			
			buildoptions { "-std=c++11" }

--			buildoptions { "`sdl2-config --cflags`" }  -- magic quotes are shell-dependent
			buildoptions { os.outputof("sdl2-config --cflags") }  -- requires GENie to be run on target machine

--			linkoptions { "`sdl2-config --libs`" } -- magic quotes are shell-dependent
			linkoptions { os.outputof("sdl2-config --libs") } -- requires GENie to be run on target machine

if _ACTION == "clean" then
	os.rmdir("../bin")
	os.rmdir("../build/.vs")		-- this doesn't seem to work because the file is write protected
	os.rmdir("../build")			-- this doesn't work because the directory contains .vs folder
end
