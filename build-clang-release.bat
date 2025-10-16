@echo off
rem Build script for Ray-Invaders with Clang (Release)
echo Building Ray-Invaders (Release)...

if not exist "bin\Release" mkdir "bin\Release" >nul 2>&1

rem Set up VS environment
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
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

if %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    exit /b %ERRORLEVEL%
)

if exist bin\Release\Ray-Invaders.exe (
    echo Build successful.
) else (
    echo Build failed: Executable not found.
    exit /b 1
)