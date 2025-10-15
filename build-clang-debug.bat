@echo off
rem Build script for Ray-Invaders with Clang
echo Building Ray-Invaders with Clang 21 (Debug)...

rem Ensure bin\Debug directory exists
if not exist "bin\Debug" mkdir "bin\Debug"

rem First, let's make sure Clang is using the Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"

rem Build with Clang using MSVC-compatible options
echo Using Clang to compile and link...
"C:\Program Files\LLVM\bin\clang++.exe" ^
  -std=c++23 ^
  -fcolor-diagnostics ^
  -fansi-escape-codes ^
  -g ^
  -Wall ^
  -Wextra ^
  -Wpedantic ^
  -D_CRT_SECURE_NO_WARNINGS ^
  -I. ^
  -Ic:\raylib\include ^
  *.cpp ^
  -v ^
  -o bin\Debug\Ray-Invaders.exe ^
  -Xlinker /LIBPATH:c:\raylib\lib ^
  -Xlinker raylib.lib ^
  -Xlinker opengl32.lib ^
  -Xlinker gdi32.lib ^
  -Xlinker winmm.lib ^
  -Xlinker user32.lib ^
  -Xlinker shell32.lib ^
  -Xlinker kernel32.lib ^
  -Xlinker /NODEFAULTLIB:libucrt.lib ^
  -Xlinker /NODEFAULTLIB:MSVCRT ^
  -Xlinker ucrt.lib ^
  -fuse-ld=lld-link

if %ERRORLEVEL% NEQ 0 (
    echo Build failed with error %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

if exist bin\Debug\Ray-Invaders.exe (
    echo Build successful! Executable created at bin\Debug\Ray-Invaders.exe
) else (
    echo Build failed: Executable not found
    exit /b 1
)

echo Build completed. Check for errors above.