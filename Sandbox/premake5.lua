project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
	{
		"src/**.h",
		"src/**.cpp",
		"**.vert",
		"**.frag"
	}

	includedirs
	{
		"%{wks.location}/Owl/vendor/spdlog/include",
		"%{wks.location}/Owl/src",
		"%{wks.location}/Owl/vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.VulkanSDK}"
	}

	links
	{
		"Owl"
	}

	filter "files:**.vert"
		buildmessage 'Compiling shader: %{file.abspath}'
		buildcommands  {
			'D:/VulkanSDK/1.3.268.0/Bin/glslc.exe %{file.abspath} -o %{wks.location}/bin/' .. outputdir .. '/%{prj.name}/%{file.reldirectory}/%{file.name}.spv'
		}
		buildoutputs { '%{wks.location}/bin/' .. outputdir .. '/%{prj.name}/%{file.reldirectory}/%{file.name}.spv' }
	filter "files:**.frag"
		buildmessage 'Compiling shader: %{file.abspath}'
		buildcommands  {
			'D:/VulkanSDK/1.3.268.0/Bin/glslc.exe %{file.abspath} -o %{wks.location}/bin/' .. outputdir .. '/%{prj.name}/%{file.reldirectory}/%{file.name}.spv'
		}
		buildoutputs { '%{wks.location}/bin/' .. outputdir .. '/%{prj.name}/%{file.reldirectory}/%{file.name}.spv' }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "OWL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "OWL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "OWL_DIST"
		runtime "Release"
		optimize "on"