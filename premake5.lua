newoption {
    trigger     = "platform-host",
    description = "(Optional) Specify host platform for cross-compilation"
}

-- remove /JMC parameter for visual studio
require('vstudio')
premake.override(premake.vstudio.vc2010.elements, "clCompile", function(oldfn, cfg)
    local calls = oldfn(cfg)
    table.insert(calls, function(cfg)
        premake.vstudio.vc2010.element("SupportJustMyCode", nil, "false")
    end)
    return calls
end)

function dofile_arg(filename, ...)
    local f = assert(loadfile(filename))
    return f(...)
end

-- Wrapper funcion around path.getabsolute() which makes drive letter lowercase on windows.
-- Otherwise drive letter can alter the case depending on environment and cause solution to reload.
function get_abs_path(p)
    p = path.getabsolute(p)
    if os.target() == "windows" then
        p = p:gsub("^%a:", function(c) return c:lower() end)
    end
    return p
end

function copy_to_targetdir(filePath)
    local filePathAbs = get_abs_path(filePath)
    postbuildcommands {"{COPY} " .. filePathAbs .. " %{cfg.targetdir}/"}
end

function get_prebuild_files()
    return { os.matchfiles("premake5.lua"), os.matchfiles("deps/*"), os.matchfiles("tools/buildscripts/*.*") }
end

function split(instr, sep)
    local substrings = {}; i = 1
    for str in string.gmatch(instr, "([^"..sep.."]+)") do
        substrings[i] = str
        i = i + 1
    end
    return substrings
end

local hostDepsDir = "_build/host-deps"
local targetDepsDir = "_build/target-deps"
local currentAbsPath = get_abs_path(".");

-- premake5.lua
workspace "ignition-omniverse1"

    configurations { "debug", "release" }
    platforms { "x86_64" }
    architecture "x86_64"

    local targetName = _ACTION
    local workspaceDir = "_compiler/"..targetName

    -- common dir name to store platform specific files
    local platform = "%{cfg.system}-%{cfg.platform}"

    local targetDependencyPlatform = "%{cfg.system}-%{cfg.platform}";
    local hostDependencyPlatform = _OPTIONS["platform-host"] or targetDependencyPlatform;

    --local externalsDir = targetDepsDir..""
    local targetDir = "_build/"..platform.."/%{cfg.buildcfg}"

    -- ignition deps
    local ignitionInstallDir = "../../install"


    -- adding dependencies
    filter { "system:linux" }
        linkoptions { '-Wl,--disable-new-dtags -Wl,-rpath,../../../_build/target-deps/nv_usd/%{cfg.buildcfg}/lib:../../../_build/target-deps/omni_client_library/%{cfg.buildcfg}:../../../_build/target-deps/python/lib:' }
        includedirs { targetDepsDir.."/nv_usd/%{cfg.buildcfg}/include", targetDepsDir.."/omni_client_library/include", targetDepsDir.."/python/include/python3.7m",
                      ignitionInstallDir.."/include",
                      ignitionInstallDir.."/include/ignition/math6",
                      ignitionInstallDir.."/include/ignition/utils1",
                      ignitionInstallDir.."/include/ignition/transport11",
                      ignitionInstallDir.."/include/ignition/msgs8",
                      ignitionInstallDir.."/include/ignition/cmake2",
                      ignitionInstallDir.."/include/ignition/common4" }
        libdirs { targetDepsDir.."/nv_usd/%{cfg.buildcfg}/lib", targetDepsDir.."/omni_client_library/%{cfg.buildcfg}", targetDepsDir.."/python/lib",
                  ignitionInstallDir.."/lib" }
    filter { "configurations:debug" }
        defines { "DEBUG", "NOMINMAX" }
        optimize "Off"
        runtime "Debug"
    filter { "configurations:release" }
        defines { "NDEBUG", "NOMINMAX" }
        optimize "On"
        runtime "Release"
    filter {}

    location (workspaceDir)
    targetdir (targetDir)

    -- symbolspath ("_build/"..targetName.."/symbols/%{cfg_buildcfg}/%{prj.name}.pdb")
    objdir ("_build/intermediate/"..platform.."/%{prj.name}")
    symbols "On"
    exceptionhandling "On"
    rtti "On"
    staticruntime "Off"
    cppdialect "C++17"

    filter { "system:linux" }
        buildoptions {"-D_GLIBCXX_USE_CXX11_ABI=0 -Wno-deprecated-declarations -Wno-deprecated -Wno-unused-variable -pthread -lstdc++fs"}
    filter {}


function sample(projectName, sourceFolder)
    project(projectName)
    kind "ConsoleApp"
    optimize "Size"
    intrinsics "off"
    inlining "Explicit"
    flags { "NoManifest", "NoIncrementalLink", "NoPCH" }
    filter { "system:linux" }
        links { "ar","arch","gf","js","kind","pcp","plug","sdf","tf","trace","usd","usdGeom", "vt","work","usdShade","usdLux","omniclient","python3.7m","boost_python37", "pthread", "stdc++fs",
                "ignition-math6", "ignition-utils1", "ignition-common4","ignition-transport11", "ignition-msgs8", "protobuf", "protoc" }
    filter {}
    location (workspaceDir.."/%{prj.name}")
    files { "source/"..sourceFolder.."/**.*" }
    filter { "system:windows" }
        links { "shlwapi" }
    filter {}
end


sample("ignition-omniverse1", "ignition_live")
