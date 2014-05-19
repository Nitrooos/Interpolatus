solution "GTKmm"

    project "Interpolatus"
        configurations { "debug", "release" }
            language "C++"
            kind "ConsoleApp"
            files "../src/*.cpp"
            buildoptions { "-w -Wall -std=c++11 `pkg-config gtkmm-3.0 --cflags` -rdynamic" }
            linkoptions { "-Wl,-rpath=../debug/ `pkg-config gtkmm-3.0 --libs` -ldl -lmpfr -liln" }
            libdirs { "../debug/" }
            objdir "../obj"

        configuration { "debug" }
            targetdir "../debug"
            flags "Symbols"

        configuration { "release" }
            targetdir "../release"
            flags "OptimizeSpeed"

        if _ACTION == "clean" then
            os.rmdir("../debug")
            os.rmdir("../release")
            os.rmdir("../obj")
        end
