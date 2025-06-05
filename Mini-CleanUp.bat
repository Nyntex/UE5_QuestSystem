@echo off

echo Kill all Unreal Tasks...
call KillAllUnrealTasks.bat

echo Delete old binaries...
rd .\Binaries /S/Q

echo Delete Solution file...
del Malice.sln