@rmdir /s /q rcp\bin

@rmdir /s /q flex\bin
@rmdir /s /q flex\obj
@rmdir /s /q flex\lib

@rmdir /s /q qt\Debug
@rmdir /s /q qt\Release
@del /q qt\Makefile
@del /q qt\Makefile.Debug
@del /q qt\Makefile.Release
@del /q qt\*.pro.user
@rmdir /s /q twentylight-build-desktop-Qt_4_7_4_for_Desktop_-_MSVC2008__Qt_SDK____

@set ROOT_PATH=%cd%\vc6
@set ORI_PATH=%cd%

@cd %ROOT_PATH%\zlib
@rmdir /S /Q Debug Release
@del /S /Q *.ncb *.opt *.suo *.exe *.aps

@cd %ROOT_PATH%\libpng
@rmdir /S /Q Debug Release
@del /S /Q *.ncb *.opt *.suo *.exe *.aps

@cd %ROOT_PATH%
@rmdir /S /Q Debug Release
@del /S /Q *.ncb *.opt *.suo *.exe *.aps

@cd %ORI_PATH%


@pause
