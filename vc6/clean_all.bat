@echo off
set ROOT_PATH=%cd%

cd %ROOT_PATH%\zlib
rmdir /S /Q Debug Release
del /S /Q *.ncb *.opt *.suo *.exe *.aps *.plg

cd %ROOT_PATH%\libpng
rmdir /S /Q Debug Release
del /S /Q *.ncb *.opt *.suo *.exe *.aps *.plg

cd %ROOT_PATH%
rmdir /S /Q Debug Release
del /S /Q *.ncb *.opt *.suo *.exe *.aps *.plg

::pause
@echo on
