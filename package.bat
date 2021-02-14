@echo off

call :package x86
call :package x86-64
call :package x86_debug
call :package x86-64_debug

:package
if not exist "build_%1" (
	exit /b
)
pushd "build_%1"
rmdir /Q /S rebuild
popd
move "build_%1" "far-extensions-%1"
exit /b 