@echo off
rem Build script for Ray-Invaders with Clang (Release)
echo Building Ray-Invaders with Clang 21 (Release)...

rem Ensure bin\Release directory exists
if not exist "bin\Release" mkdir "bin\Release"

rem First, let's make sure Clang is using the Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"

rem Build with Clang using MSVC-compatible options
"C:\Program Files\LLVM\bin\clang++.exe" ^
  -std=c++23 ^
  -fcolor-diagnostics ^
  -fansi-escape-codes ^
  -O3 ^
  -Wall ^
  -Wextra ^
  -Wpedantic ^
  -D_CRT_SECURE_NO_WARNINGS ^
  -I. ^
  -Ic:\raylib\include ^
  *.cpp ^
  -o bin\Release\Ray-Invaders.exe ^
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
  -Xlinker /LTCG ^
  -fuse-ld=lld-link

echo Build completed. Check for errors above.