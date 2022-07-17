@echo off

rem IF NOT EXIST d:\programming\gui\src\build mkdir d:\programming\gui\src\build
set OUT_DIR=build
set OUT_EXE=main
rem set INCLUDES=/ID:\programming\gui\src\imgui  /ID:\programming\gui\src\imgui\backends /ID:\programming\gui\src\libs\glfw\include
set INCLUDES=/ID:\programming\gui\src\imgui /ID:\programming\gui\src\implot /ID:\programming\gui\src\imgui\backends /ID:\programming\gui\src\libs\glfw\include
set LIBS=/LIBPATH:D:\programming\gui\src\libs\glfw\lib-vc2019 glfw3.lib opengl32.lib gdi32.lib shell32.lib
set SOURCES=d:\programming\gui\src\main.cpp d:\programming\gui\src\imgui\backends\imgui_impl_glfw.cpp d:\programming\gui\src\imgui\backends\imgui_impl_opengl3.cpp d:\programming\gui\src\imgui\imgui*.cpp d:\programming\gui\src\implot\implot*.cpp
IF NOT EXIST %OUT_DIR% mkdir %OUT_DIR%
cl /nologo /W3 /Zi /MD %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

rem pushd d:\programming\gui\src\build
rem rem cl %INCLUDES% %SOURCES% /link %LIBS%

rem popd

rem @set OUT_DIR=build
rem @set OUT_EXE=main
rem @set INCLUDES=/ID:\programming\gui\src\imgui\ /ID:\programming\gui\src\imgui\backends /ID:\programming\gui\src\libs\glfw\include
rem @set SOURCES=D:\programming\gui\src\main.cpp D:\programming\gui\src\imgui\backends\imgui_impl_glfw.cpp D:\programming\gui\src\imgui\backends\imgui_impl_opengl3.cpp D:\programming\gui\src\imgui\imgui*.cpp
rem @set LIBS=/LIBPATH:D:\programming\gui\src\libs\glfw\lib-vc2019 glfw3.lib opengl32.lib gdi32.lib shell32.lib
rem mkdir %OUT_DIR%
rem cl /nologo /Zi /MD %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%
