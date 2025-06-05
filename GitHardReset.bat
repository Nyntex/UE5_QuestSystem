@echo This Commandchain is to fix unremovable files from Git. 
@echo Make sure you have everything commit you want to have comitted because this
@echo will hard reset your git to the latest version.
@echo Press Enter to continue, this may take a while!

@ECHO OFF
pause -s
@echo Now marking all items as remove, don't worry, they will not be removed!
git rm --cached -r .
@echo
@echo Now restting to latest version, reverting all changes you had!
@echo
git reset --hard
pause