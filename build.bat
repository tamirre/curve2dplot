@echo off

set OUT_DIR=..\build-win32
set BIN_DIR=..\bin
set OUT_EXE=main
set INCLUDES=/IC:\programming\gui\src\imgui-docking /IC:\programming\gui\src\implot /IC:\programming\gui\src\ImGuiFileDialog /IC:\programming\gui\src\imgui-docking\backends /IC:\programming\gui\src\lib\glfw\include
set LIBS=/LIBPATH:C:\programming\gui\src\lib\glfw\lib-vc2019 glfw3.lib opengl32.lib gdi32.lib shell32.lib
set SOURCES=C:\programming\gui\src\main.cpp C:\programming\gui\src\imgui-docking\backends\imgui_impl_glfw.cpp C:\programming\gui\src\imgui-docking\backends\imgui_impl_opengl2.cpp  C:\programming\gui\src\implot\implot*.cpp  C:\programming\gui\src\imgui-docking\imgui*.cpp
IF NOT EXIST %OUT_DIR% mkdir %OUT_DIR%
cl /nologo /W3 /Zi /MD %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

IF NOT EXIST %BIN_DIR% mkdir %BIN_DIR%
@copy %OUT_DIR%\%OUT_EXE%.exe %BIN_DIR%\%OUT_EXE%.exe
