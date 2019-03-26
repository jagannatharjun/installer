REM C:\Qt\5.12.1\msvc2017-x86-static\bin\qmake.exe ..\ "CONFIG += static release" 
if NOT %errorlevel% == 0 goto error
jom
if NOT %errorlevel% == 0 goto error
ninja compiler uninstaller
if NOT %errorlevel% == 0 goto error
copy compiler\compiler.exe deploy\compiler.exe 
copy uninstaller\uninstaller.exe deploy\private\uninstaller_res.exe
if NOT %errorlevel% == 0 goto error
copy release\installer.exe deploy\installer_res.exe
if NOT %errorlevel% == 0 goto error
cd deploy
mt.exe -manifest Deploy\1.manifest -outputresource:Deploy\installer_res.exe;1
mt.exe -manifest Deploy\1.manifest -outputresource:Deploy\private\uninstaller_res.exe;1
if NOT %errorlevel% == 0 goto error
compiler.exe
if NOT %errorlevel% == 0 goto error
installer.exe
if NOT %errorlevel% == 0 goto error
cd ..

:error
set errorlevel=0