VULKAN_SDK = os.getenv("Vulkan_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/Owl/vendor/stb_image"
IncludeDir["GLFW"] = "%{wks.location}/Owl/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Owl/vendor/Glad/include"
IncludeDir["glm"] = "%{wks.location}/Owl/vendor/glm"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/lib"

Library = {}

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"
