@echo off

echo Kill all Unreal Tasks...
call KillAllUnrealTasks.bat

echo Delete Temporary Directories...
 rd .\Binaries /S/Q
 rd .\Debug /S/Q
 rd .\Intermediate /S/Q
 rd .\DerivedDataCache /S/Q
 del .\Saved\AutoScreenshot.png
 rd .\Saved\Autosaves /S/Q
 rd .\Saved\Crashes /S/Q
 rd .\Saved\Logs /S/Q
 rd .\Saved\Collections /S/Q
 rd .\Saved\Cooked /S/Q
 rd .\Saved\Crashes /S/Q
 rd .\Saved\Screenshots /S/Q
 rd .\Saved\StagedBuilds /S/Q
 rd .\Saved\Temp /S/Q
 rd .\Saved\AssetData /S/Q
 rd .\Saved\Config\CrashReportClient /S/Q

echo Delete Solution file...
del Malice.sln

echo Delete Temporary Plugin Directories...
for /D %%s in (.\Plugins\*) do ( del %%s\Binaries\Win64\UE4Editor*.* )
for /D %%s in (.\Plugins\*) do ( rmdir %%s\Intermediate /s/q )