@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64

set OUT_DIR=%~dp0\build-win32
set BIN_DIR=%~dp0\bin
set OUT_EXE=curve2dplot
rem 
set INCLUDES=/I%~dp0\src\imgui-docking /I%~dp0\src\ImGuiFileDialog\dirent /I%~dp0\src\implot /I%~dp0\src\imgui-docking\backends /I%~dp0\src\lib\glfw\include /I%~dp0\src\ImGuiFileDialog
set LIBS=/LIBPATH:%~dp0\src\lib\glfw\lib-vc2019 glfw3.lib opengl32.lib gdi32.lib shell32.lib
set SOURCES=%~dp0\src\curve2dplot.cpp %~dp0\src\imgui-docking\backends\imgui_impl_glfw.cpp %~dp0\src\imgui-docking\backends\imgui_impl_opengl2.cpp %~dp0\src\imgui-docking\imgui*.cpp %~dp0\src\implot\implot*.cpp %~dp0\src\ImGuiFileDialog\ImGuiFileDialog.cpp
IF NOT EXIST %OUT_DIR% mkdir %OUT_DIR%
rem /O2 /Ob3 /G 
cl /nologo /Zi /MD /EHsc %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

IF NOT EXIST %BIN_DIR% mkdir %BIN_DIR%
@copy %OUT_DIR%\%OUT_EXE%.exe %BIN_DIR%\%OUT_EXE%.exe