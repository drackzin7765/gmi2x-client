@echo off
title [VCODLIB] WINDOWS - DEBUG
cls
echo BUILDING...
md objects


echo ================================================================
echo                  BUILDING GMI2x-Client.DLL

echo ##### COMPILE HOOKING.CPP #####
g++ -c -g hooking.cpp -o objects/hooking.opp

echo #### COMPILE CVAR.CPP ####
g++ -c -g cvar.cpp -o objects/cvar.opp

echo ##### COMPILE FUNCTIONS.CPP #####
g++ -c -g functions.cpp -o objects/functions.opp

echo ##### COMPILE CL_COMMAND.CPP #####
g++ -c -g cl_command.cpp -o objects/cl_command.opp

rem echo ##### COMPILE QVSNPRINTF.C #####
rem g++ -c -g .\lib\qvsnprintf.c -o objects/qvsnprintf.opp

echo ##### COMPILE CLIENT.CPP #####
g++ -c -g client.cpp -o objects/client.opp

echo #### COMPILE GAME.CPP ####
g++ -c -g game.cpp -o objects/game.opp

echo #### COMPILE CGAME.CPP ####
g++ -c -g cgame.cpp -o objects/cgame.opp

echo ##### COMPILE GMI2X-CL.CPP #####
g++ -c -g gmi2x-cl.cpp -o objects/gmi2x-cl.opp

echo #### COMPILE GMI2x-Client.EXE ####
windres resources.rc -O coff -o objects/resources.o
g++ -o ..\bin\gmi2x-client.exe injector.cpp objects/resources.o -mwindows -lcomdlg32 -luser32 -lkernel32 -ladvapi32

echo ##### LINK GMI2X-CLIENT.dll #####
g++ -m32 -shared -o ..\bin\gmi2x-client.dll objects\*.opp -lwinmm


echo                           DONE
echo ================================================================


rd /s /q objects

echo BUILDING DONE!
pause
