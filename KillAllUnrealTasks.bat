@echo off

WMIC PROCESS WHERE name="UnrealEditor.exe" CALL TERMINATE
WMIC PROCESS WHERE name="UnrealEditor-Win64-DebugGame.exe" CALL TERMINATE

taskkill /IM UnrealEditor.exe /F
taskkill /IM UnrealEditor-Win64-DebugGame.exe /F
