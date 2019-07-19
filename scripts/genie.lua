solution "sdl2-tetris"
	location "../build"
	configurations { "Debug", "Dev", "Release" }
	platforms { "native", "x32", "x64" }

	project "sdl2-tetris"
		location "../build"
		kind "ConsoleApp"
		language "C++"
		files { "../src/**.h", "../src/**.cpp" }
		flags { "ExtraWarnings", "FatalWarnings" }
		links { "SDL2_ttf" } -- not in the string returned by `sdl2-config --links`
		debugdir "../data"		-- debugger working directory. Not implemented for Xcode so use must set manually.
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }
			targetdir "../bin/debug"
		
		configuration "Dev"
			flags { "Optimize", "Symbols" }			
			targetdir "../bin/dev"

		configuration "Release"
			defines { "NDEBUG", "RELEASE" }
			flags { "Optimize" }
			targetdir "../bin/release"

		configuration "windows"
			-- There is a single SDLmain.lib that is built against the Multi-threaded DLL (/MD) i.e. there is no debug lib built against Multi-threaded Debug DLL (/MD
			flags { "ReleaseRuntime" } 
			
			includedirs { "../3rdparty/SDL2-2.0.3/include" }
			includedirs { "../3rdparty/SDL2_ttf-2.0.12/include" }
			links { "SDL2", "SDL2main" }
			
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
			
			buildoptions { "-std=c++0x" }
			buildoptions { "-Wno-switch" }
			buildoptions { "-Wno-unused-function" }
			buildoptions { "-Wno-missing-field-initializers" }
			buildoptions { "-Wno-missing-braces" }
			
			-- TODO: Comment on why using sdl2-config rather than magic quotes on Linux
--			buildoptions { "`sdl2-config --cflags`" }  -- magic quotes are shell-dependent
--			linkoptions { "`sdl2-config --libs`" } -- magic quotes are shell-dependent
if os.get() == "linux" then
			buildoptions { os.outputof("sdl2-config --cflags") }  -- requires GENie to be run on target machine
--			linkoptions { os.outputof("sdl2-config --libs") } -- requires GENie to be run on target machine
end
			libdirs { "/opt/vc/lib" } -- really just Raspberry Pi only (VideoCore) 
			links { "EGL", "GLESv2" }

		configuration "macosx"
			buildoptions { "-std=c++11" }
			buildoptions { "-Wno-unused-function" }
			buildoptions { "-Wno-missing-braces" }

		configuration { "macosx", "xcode*" }
if os.get() == "macosx" then
			buildoptions { os.outputof("sdl2-config --cflags") } -- magic quotes are no good for Xcode so can't use `sdl2-config --cflags`
			linkoptions { os.outputof("sdl2-config --libs") }
end

		configuration { "macosx", "not xcode*" }
			buildoptions { "`sdl2-config --cflags`" } -- magic quotes are no good for Xcode
			linkoptions { "`sdl2-config --libs`" }

newaction
{
	trigger = "clean",
	shortname = "clean",
	description = "Removes generated files."
}
			
if _ACTION == "clean" then
	os.rmdir("../bin")
	if os.get() == "windows" then
		os.outputof("rmdir ..\\build\\.vs /s /q")
	end
	if os.get() == "macosx" then
		os.outputof("rm -rf build") -- remove the build folder, including hidden .DS_Store file
	end

	os.rmdir("../build")
end
