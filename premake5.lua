include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "OwlEngine"
    architecture "x86_64"
    startproject "Sandbox"

    configurations 
    { 
        "Debug", 
        "Release", 
        "Dist" 
    }

	solution_items
	{
		".editorconfig"
	}

    flags 
    {
		"MultiProcessorCompile"
	}
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "vendor/premake"
    include "Owl/vendor/GLFW"
group ""

group "Core"
    include "Owl"
    include "Owl-ScriptCore"
group ""

group "Tools"
    include "OwlEngine"
group ""

group "Misc"
    include "Sandbox"
group ""