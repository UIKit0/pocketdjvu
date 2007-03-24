@rem %1 - path to EditIni.exe
@rem %2 - $(TargetName)

@set EI=%1
@set TargetName=%2
@set FromPath=..\..\..\Info\
@set EZ=..\..\..\Util\ezsetup.exe 

@copy %FromPath%AppMgr.ini .\AppMgr.ini
@copy ..\..\res\PocketDjvu.ico .\PocketDjvu.ico

%EI% .\AppMgr.ini /sec PocketDjVu /key DeviceFile /val %TargetName%.exe
%EI% .\AppMgr.ini /sec PocketDjVu /key CabFiles   /val %TargetName%.ARM.CAB

%EZ% -l english -i AppMgr.ini -r %FromPath%readme.txt -e %FromPath%eula.txt -o PocketDjVu_Setup.exe