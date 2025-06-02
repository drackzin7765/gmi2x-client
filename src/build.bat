@echo off
title [VCODLIB] WINDOWS - DEBUG
cls
echo BUILDING...
md objects


echo ================================================================
echo                  BUILDING CUOCX.DLL

echo ##### COMPILE HOOKING.CPP #####
g++ -c -m32 -g hooking.cpp -o objects/hooking.opp

echo #### COMPILE CGAME.CPP ####
g++ -c -m32 -g cgame.cpp -o objects/cgame.opp

echo #### COMPILE GAME.CPP ####
g++ -c -m32 -g game.cpp -o objects/game.opp

echo #### COMPILE CVAR.CPP ####
g++ -c -m32 -g cvar.cpp -o objects/cvar.opp

echo ##### COMPILE FUNCTIONS.CPP #####
g++ -c -m32 -g functions.cpp -o objects/functions.opp

echo ##### COMPILE CL_COMMAND.CPP #####
g++ -c -m32 -g cl_command.cpp -o objects/cl_command.opp

rem echo ##### COMPILE QVSNPRINTF.C #####
rem g++ -c -g .\lib\qvsnprintf.c -o objects/qvsnprintf.opp

echo ##### COMPILE CLIENT.CPP #####
g++ -c -m32 -g client.cpp -o objects/client.opp

echo ##### COMPILE CUOCX.CPP #####
g++ -c -m32 -g cuocx.cpp -o objects/cuocx.opp

echo #### COMPILE CUOCX_INJ.EXE ####
windres resources.rc -O coff -o objects/resources.o
g++ -o ..\bin\cuocx.exe injector.cpp objects/resources.o -mwindows -lcomdlg32 -luser32 -lkernel32 -ladvapi32

echo ##### LINK CUOCX_LIB.dll #####
g++ -m32 -shared -o ..\bin\cuocx_lib.dll objects\*.opp -lwinmm -lImageHlp


echo                           DONE
echo ================================================================


rd /s /q objects

echo BUILDING DONE!
pause
