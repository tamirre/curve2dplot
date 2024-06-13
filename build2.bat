@echo off

set OUT_DIR=..\build-win32
set BIN_DIR=..\bin
set OUT_EXE=test_mpl
rem 
rem set INCLUDES=/IC:\projects\gui\src\matplot /IC:\projects\gui\src\matplot\util
set INCLUDES=/I. /IC:\projects\gui\src\matplot /IC:\projects\gui\src\matplot\backend /IC:\projects\gui\src\matplot\axes_objects /IC:\projects\gui\src\matplot\core /IC:\projects\gui\src\matplot\freestanding /IC:\projects\gui\src\matplot\util /IC:\projects\gui\src\glad\include\ /IC:\projects\gui\src\lib\glfw\include /IC:\projects\gui\src\3rd_party
rem /IC:\projects\gui\src\matplot\backend /IC:\projects\gui\src\matplot\axes_objects /IC:\projects\gui\src\matplot\core /IC:\projects\gui\src\matplot\freestanding /IC:\projects\gui\src\matplot\util
set LIBS=/LIBPATH:C:\projects\gui\src\lib\glfw\lib-vc2019 C:\projects\gui\src\lib\matplot
rem glfw3.lib opengl32.lib gdi32.lib shell32.lib
set SOURCES=C:\projects\gui\src\test_mpl.cpp C:\projects\gui\src\matplot\core\*.cpp C:\projects\gui\src\matplot\backend\*.cpp C:\projects\gui\src\matplot\axes_objects\*.cpp C:\projects\gui\src\matplot\freestanding\*.cpp C:\projects\gui\src\matplot\util\*.cpp C:\projects\gui\src\3rd_party\src\*.cpp
IF NOT EXIST %OUT_DIR% mkdir %OUT_DIR%
rem /O2 /Ob3 /G

cl /nologo /std:c++17 /Zi /MD /EHsc %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ 

IF NOT EXIST %BIN_DIR% mkdir %BIN_DIR%
@copy %OUT_DIR%\%OUT_EXE%.exe %BIN_DIR%\%OUT_EXE%.exe


rem @echo off

rem set OUT_DIR=..\build-win32
rem set BIN_DIR=..\bin
rem set OUT_EXE=main
rem rem 
rem set INCLUDES=/IC:\projects\gui\src\imgui-docking /IC:\projects\gui\src\ImGuiFileDialog\dirent /IC:\projects\gui\src\implot /IC:\projects\gui\src\imgui-docking\backends /IC:\projects\gui\src\lib\glfw\include /IC:\projects\gui\src\ImGuiFileDialog
rem set LIBS=/LIBPATH:C:\projects\gui\src\lib\glfw\lib-vc2019 glfw3.lib opengl32.lib gdi32.lib shell32.lib
rem set SOURCES=C:\projects\gui\src\main.cpp C:\projects\gui\src\imgui-docking\backends\imgui_impl_glfw.cpp C:\projects\gui\src\imgui-docking\backends\imgui_impl_opengl2.cpp C:\projects\gui\src\imgui-docking\imgui*.cpp C:\projects\gui\src\implot\implot*.cpp C:\projects\gui\src\ImGuiFileDialog\ImGuiFileDialog.cpp
rem IF NOT EXIST %OUT_DIR% mkdir %OUT_DIR%
rem rem /O2 /Ob3 /G 
rem cl /nologo /Zi /MD /EHsc %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

rem IF NOT EXIST %BIN_DIR% mkdir %BIN_DIR%
rem @copy %OUT_DIR%\%OUT_EXE%.exe %BIN_DIR%\%OUT_EXE%.exe









rem rem set OUT_DIR=..\build-win32
rem rem set BIN_DIR=..\bin
rem rem set OUT_EXE=test

rem rem set INCLUDES=/IC:\projects\gui\src\imgui-docking /IC:\projects\gui\src\ImGuiFileDialog\dirent /IC:\projects\gui\src\implot /IC:\projects\gui\src\imgui-docking\backends /IC:\projects\gui\src\lib\glfw\include /IC:\projects\gui\src\ImGuiFileDialog
rem rem rem /I"C:\Program Files\ActiveState\include"
rem rem rem set LIBS=/LIBPATH:"C:\Program Files\ActiveState\lib\tcllibc\win32-x86_64" tcllibc.dll
rem rem set LIBS=/LIBPATH:C:\projects\gui\src\lib\glfw\lib-vc2019 glfw3.lib opengl32.lib gdi32.lib shell32.lib
rem rem          rem /LIBPATH:"C:\Program Files\ActiveState\lib\" tcl86t.lib
rem rem set SOURCES=C:\projects\gui\src\test.cpp C:\projects\gui\src\imgui-docking\backends\imgui_impl_glfw.cpp C:\projects\gui\src\imgui-docking\backends\imgui_impl_opengl2.cpp
rem rem IF NOT EXIST %OUT_DIR% mkdir %OUT_DIR%
rem rem  rem /O2 /Ob3 /G 
rem rem cl /nologo /Zi /MD /clr %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

rem rem IF NOT EXIST %BIN_DIR% mkdir %BIN_DIR%
rem rem @copy %OUT_DIR%\%OUT_EXE%.exe %BIN_DIR%\%OUT_EXE%.exe


rem set OUT_DIR=..\build-win32
rem set BIN_DIR=..\bin
rem set OUT_EXE=test
rem rem 
rem set INCLUDES=/IC:\projects\gui\src\imgui-docking /IC:\projects\gui\src\ImGuiFileDialog\dirent /IC:\projects\gui\src\implot /IC:\projects\gui\src\imgui-docking\backends /IC:\projects\gui\src\lib\glfw\include /IC:\projects\gui\src\ImGuiFileDialog /I"C:\Program Files\ActiveState\include"
rem set LIBS=/LIBPATH:C:\projects\gui\src\lib\glfw\lib-vc2019 glfw3.lib opengl32.lib gdi32.lib shell32.lib /LIBPATH:"C:\Program Files\ActiveState\lib" tcl86t.lib
rem set SOURCES=C:\projects\gui\src\test.cpp
rem IF NOT EXIST %OUT_DIR% mkdir %OUT_DIR%
rem  rem /O2 /Ob3 /G 
rem cl /nologo /clr %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%
rem IF NOT EXIST %BIN_DIR% mkdir %BIN_DIR%
rem @copy %OUT_DIR%\%OUT_EXE%.exe %BIN_DIR%\%OUT_EXE%.exe