@echo off
set shader=%1
set output=%~dp1compiled\%~dpn1.spv
mkdir %~dp1compiled 2>nul
D:/VulkanSDK/1.3.268.0/bin/glslc.exe %shader% -o %output%