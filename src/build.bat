@echo off

@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
rem @set OUT_DIR=Build
rem @set OUT_EXE=main
rem @set INCLUDES=/I.\imgui /I "C:\Program Files\Microsoft SDKs\Windows\v7.0\Include"
rem @set SOURCES=main.cpp .\imgui\imgui_impl_dx11.cpp .\imgui\imgui_impl_win32.cpp .\imgui\imgui*.cpp
rem @set LIBS=/LIBPATH:"C:\Program Files\Microsoft SDKs\Windows\v7.0\Lib" d3d11.lib d3dcompiler.lib
rem mkdir %OUT_DIR%
rem "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\vcvarsx86_amd64.bat" & cl /nologo /Zi /MD %INCLUDES% /D UNICODE /D _UNICODE %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

rem C:\Program Files\Microsoft SDKs\Windows\v7.0\Lib
@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
@set OUT_DIR=Build
@set OUT_EXE=main
@set INCLUDES=/I.\libs\glfw\include /I.\imgui\
@set SOURCES=main.cpp .\imgui\backends\imgui_impl_glfw.cpp .\imgui\backends\imgui_impl_opengl3.cpp .\imgui\imgui*.cpp
@set LIBS=/LIBPATH:.\libs\glfw\lib-vc2010-32 glfw3.lib opengl32.lib gdi32.lib shell32.lib
mkdir %OUT_DIR%
"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\vcvarsx86_amd64.bat" & "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\x86_amd64\cl.exe" /EHsc /nologo /Zi /MD %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%


rem IF EXIST E:\tamir\gui\src\imgui SET "PATH=%PATH%E:\tamir\gui\src\imgui;C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\include;"
rem rem echo %PATH%
rem mkdir build
rem pushd build
rem "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\x86_amd64\cl.exe" /EHsc -Zi e:\tamir\gui\src\main.cpp
rem popd
