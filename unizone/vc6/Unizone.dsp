# Microsoft Developer Studio Project File - Name="Unizone" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Unizone - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Unizone.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Unizone.mak" CFG="Unizone - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Unizone - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Unizone - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(QTDIR)\include" /I "..\src" /I "..\src\muscle" /I "..\libjpeg" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "BETA" /D "QT_NO_ASCII_CAST" /D "UNICODE" /D "MUSCLE_ENABLE_ZLIB_ENCODING" /D "ZLIB_USEDLL" /D "REGEX_USEDLL" /Fr /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 muscle.lib zlib.lib regex.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib shlwapi.lib oleaut32.lib uuid.lib wsock32.lib winmm.lib qt-mt230nc.lib qtmain.lib qjpeg.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcd" /out:"Unizone.exe" /libpath:"$(QTDIR)\lib" /libpath:"..\libjpeg\Release"
# SUBTRACT LINK32 /pdb:none /incremental:yes

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "$(QTDIR)\include" /I "..\src" /I "..\src\muscle" /I "..\libjpeg" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /D "QT_NO_ASCII_CAST" /D "BETA" /D "UNICODE" /D "MUSCLE_ENABLE_ZLIB_ENCODING" /D "ZLIB_USEDLL" /D "REGEX_USEDLL" /D "DISABLE_OBJECT_POOLING" /FR /FD /GZ /c
# SUBTRACT CPP /X /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/Unizoned.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winspool.lib odbc32.lib odbccp32.lib muscled.lib zlibd.lib regexd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib shlwapi.lib oleaut32.lib uuid.lib wsock32.lib winmm.lib qt-mt230nc.lib qtmain.lib qjpegd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"Unizoned.exe" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"..\libjpeg\Debug"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "Unizone - Win32 Release"
# Name "Unizone - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\aboutdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\aboutdlgimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\botitem.cpp
# End Source File
# Begin Source File

SOURCE=..\src\channel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\channelimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\channelinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\chattext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\chatwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\colors.cpp
# End Source File
# Begin Source File

SOURCE=..\src\combo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\downloadimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\downloadthread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\downloadworker.cpp
# End Source File
# Begin Source File

SOURCE=..\src\fileinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\filethread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\formatting.cpp
# End Source File
# Begin Source File

SOURCE=..\src\htmlview.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Log.cpp
# End Source File
# Begin Source File

SOURCE=..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\src\md5.cpp
# End Source File
# Begin Source File

SOURCE=..\src\menubar.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_aboutdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_aboutdlgimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_channel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_channelimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_chattext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_combo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_downloadimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_downloadthread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_formatting.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_htmlview.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_menubar.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_netclient.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_picviewer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_picviewerimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_prefs.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_prefsimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_privatewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_privatewindowimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\muscle\qtsupport\moc_QAcceptSocketsThread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\muscle\qtsupport\moc_QMessageTransceiverThread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_scanprogress.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_serverclient.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_ulistview.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_updateclient.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_uploadthread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\moc_winsharewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\netclient.cpp
# End Source File
# Begin Source File

SOURCE=..\src\nicklist.cpp
# End Source File
# Begin Source File

SOURCE=..\src\picviewer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\picviewerimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\platform.cpp
# End Source File
# Begin Source File

SOURCE=..\src\prefs.cpp
# End Source File
# Begin Source File

SOURCE=..\src\prefsimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\privatewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\privatewindowimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\muscle\qtsupport\QAcceptSocketsThread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\muscle\qtsupport\QMessageTransceiverThread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scanprogress.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scanprogressimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\searchitem.cpp
# End Source File
# Begin Source File

SOURCE=..\src\serverclient.cpp
# End Source File
# Begin Source File

SOURCE=..\src\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\textevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\titanic.cpp
# End Source File
# Begin Source File

SOURCE=..\src\transferitem.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ulistview.cpp
# End Source File
# Begin Source File

SOURCE=..\src\updateclient.cpp
# End Source File
# Begin Source File

SOURCE=..\src\uploadthread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\user.cpp
# End Source File
# Begin Source File

SOURCE=..\src\userlistitem.cpp
# End Source File
# Begin Source File

SOURCE=..\src\util.cpp
# End Source File
# Begin Source File

SOURCE=..\src\version.cpp
# End Source File
# Begin Source File

SOURCE=..\src\werrorevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\winshare_channels.cpp
# End Source File
# Begin Source File

SOURCE=..\src\winshare_lists.cpp
# End Source File
# Begin Source File

SOURCE=..\src\winshare_network.cpp
# End Source File
# Begin Source File

SOURCE=..\src\winshare_parsing.cpp
# End Source File
# Begin Source File

SOURCE=..\src\winshare_search.cpp
# End Source File
# Begin Source File

SOURCE=..\src\winshare_slots.cpp
# End Source File
# Begin Source File

SOURCE=..\src\winsharewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wpwevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wstatusbar.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wstring.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wsystemevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wwarningevent.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\aboutdlg.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\aboutdlg.h
InputName=aboutdlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\aboutdlg.h
InputName=aboutdlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\aboutdlgimpl.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\aboutdlgimpl.h
InputName=aboutdlgimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\aboutdlgimpl.h
InputName=aboutdlgimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\botitem.h
# End Source File
# Begin Source File

SOURCE=..\src\channel.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\channel.h
InputName=channel

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\channel.h
InputName=channel

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\channelimpl.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\channelimpl.h
InputName=channelimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\channelimpl.h
InputName=channelimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\channelinfo.h
# End Source File
# Begin Source File

SOURCE=..\src\chattext.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\chattext.h
InputName=chattext

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\chattext.h
InputName=chattext

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\chatwindow.h
# End Source File
# Begin Source File

SOURCE=..\src\colors.h
# End Source File
# Begin Source File

SOURCE=..\src\combo.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\combo.h
InputName=combo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\combo.h
InputName=combo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\debugimpl.h
# End Source File
# Begin Source File

SOURCE=..\src\downloadimpl.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\downloadimpl.h
InputName=downloadimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\downloadimpl.h
InputName=downloadimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\downloadthread.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\downloadthread.h
InputName=downloadthread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\downloadthread.h
InputName=downloadthread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\downloadworker.h
# End Source File
# Begin Source File

SOURCE=..\src\fileinfo.h
# End Source File
# Begin Source File

SOURCE=..\src\filethread.h
# End Source File
# Begin Source File

SOURCE=..\src\formatting.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\formatting.h
InputName=formatting

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\formatting.h
InputName=formatting

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\global.h
# End Source File
# Begin Source File

SOURCE=..\src\gotourl.h
# End Source File
# Begin Source File

SOURCE=..\src\htmlview.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\htmlview.h
InputName=htmlview

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\htmlview.h
InputName=htmlview

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\Log.h
# End Source File
# Begin Source File

SOURCE=..\src\md5.h
# End Source File
# Begin Source File

SOURCE=..\src\menubar.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\menubar.h
InputName=menubar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\menubar.h
InputName=menubar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\netclient.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\netclient.h
InputName=netclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\netclient.h
InputName=netclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\nicklist.h
# End Source File
# Begin Source File

SOURCE=..\src\picviewer.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\picviewer.h
InputName=picviewer

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\picviewer.h
InputName=picviewer

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\picviewerimpl.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\picviewerimpl.h
InputName=picviewerimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\platform.h
# End Source File
# Begin Source File

SOURCE=..\src\prefs.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\prefs.h
InputName=prefs

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\prefs.h
InputName=prefs

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\prefsimpl.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\prefsimpl.h
InputName=prefsimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\prefsimpl.h
InputName=prefsimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\privatewindow.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\privatewindow.h
InputName=privatewindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\privatewindow.h
InputName=privatewindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\privatewindowimpl.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\privatewindowimpl.h
InputName=privatewindowimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\privatewindowimpl.h
InputName=privatewindowimpl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\muscle\qtsupport\QAcceptSocketsThread.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src\muscle\qtsupport
InputPath=..\src\muscle\qtsupport\QAcceptSocketsThread.h
InputName=QAcceptSocketsThread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src\muscle\qtsupport
InputPath=..\src\muscle\qtsupport\QAcceptSocketsThread.h
InputName=QAcceptSocketsThread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\muscle\qtsupport\QMessageTransceiverThread.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src\muscle\qtsupport
InputPath=..\src\muscle\qtsupport\QMessageTransceiverThread.h
InputName=QMessageTransceiverThread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src\muscle\qtsupport
InputPath=..\src\muscle\qtsupport\QMessageTransceiverThread.h
InputName=QMessageTransceiverThread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\resource.h
# End Source File
# Begin Source File

SOURCE=..\src\scanevent.h
# End Source File
# Begin Source File

SOURCE=..\src\scanprogress.h
# End Source File
# Begin Source File

SOURCE=..\src\scanprogressimpl.h
# End Source File
# Begin Source File

SOURCE=..\src\searchitem.h
# End Source File
# Begin Source File

SOURCE=..\src\serverclient.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\serverclient.h
InputName=serverclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\serverclient.h
InputName=serverclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\settings.h
# End Source File
# Begin Source File

SOURCE=..\src\textevent.h
# End Source File
# Begin Source File

SOURCE=..\src\titanic.h
# End Source File
# Begin Source File

SOURCE=..\src\tokenizer.h
# End Source File
# Begin Source File

SOURCE=..\src\transferitem.h
# End Source File
# Begin Source File

SOURCE=..\src\transferlist.h
# End Source File
# Begin Source File

SOURCE=..\src\ulistview.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\ulistview.h
InputName=ulistview

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\ulistview.h
InputName=ulistview

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\updateclient.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\updateclient.h
InputName=updateclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\updateclient.h
InputName=updateclient

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\uploadthread.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\uploadthread.h
InputName=uploadthread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\uploadthread.h
InputName=uploadthread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\user.h
# End Source File
# Begin Source File

SOURCE=..\src\userlistitem.h
# End Source File
# Begin Source File

SOURCE=..\src\util.h
# End Source File
# Begin Source File

SOURCE=..\src\version.h
# End Source File
# Begin Source File

SOURCE=..\src\wdownloadevent.h
# End Source File
# Begin Source File

SOURCE=..\src\werrorevent.h
# End Source File
# Begin Source File

SOURCE=..\src\winsharewindow.h

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\winsharewindow.h
InputName=winsharewindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=\build\unizone\src
InputPath=..\src\winsharewindow.h
InputName=winsharewindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\windows\wlaunchthread_win.h
# End Source File
# Begin Source File

SOURCE=..\src\wpwevent.h
# End Source File
# Begin Source File

SOURCE=..\src\wstatusbar.h
# End Source File
# Begin Source File

SOURCE=..\src\wstring.h
# End Source File
# Begin Source File

SOURCE=..\src\wsystemevent.h
# End Source File
# Begin Source File

SOURCE=..\src\wuploadevent.h
# End Source File
# Begin Source File

SOURCE=..\src\wwarningevent.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\src\icon.rc
# End Source File
# Begin Source File

SOURCE=..\src\unizone.ico
# End Source File
# Begin Source File

SOURCE=..\src\unizone.pro
# End Source File
# End Group
# Begin Group "UI files"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=..\src\aboutdlg.ui

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\aboutdlg.ui
InputName=aboutdlg

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\aboutdlg.ui
InputName=aboutdlg

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\channel.ui

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\channel.ui
InputName=channel

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\channel.ui
InputName=channel

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\picviewer.ui

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\picviewer.ui
InputName=picviewer

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\picviewer.ui
InputName=picviewer

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\prefs.ui

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\prefs.ui
InputName=prefs

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\prefs.ui
InputName=prefs

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\privatewindow.ui

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\privatewindow.ui
InputName=privatewindow

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\privatewindow.ui
InputName=privatewindow

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\scanprogress.ui

!IF  "$(CFG)" == "Unizone - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\scanprogress.ui
InputName=scanprogress

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=\build\unizone\src
InputPath=..\src\scanprogress.ui
InputName=scanprogress

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "DLL files"

# PROP Default_Filter ".dll"
# Begin Source File

SOURCE=..\libjpeg\Release\qjpeg.dll

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Copying $(InputName).dll from $(InputDir) to $(TargetDir)
InputDir=\build\unizone\libjpeg\Release
TargetDir=\build\unizone\vc6
TargetName=Unizone
InputPath=..\libjpeg\Release\qjpeg.dll
InputName=qjpeg

"$(TargetDir)\qjpeg.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).dll" "$(TargetDir)\$(TargetName).dll"

# End Custom Build

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\libjpeg\Debug\qjpegd.dll

!IF  "$(CFG)" == "Unizone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Unizone - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Copying $(InputName).dll from $(InputDir) to $(TargetDir)
InputDir=\build\unizone\libjpeg\Debug
TargetDir=\build\unizone\vc6
TargetName=Unizoned
InputPath=..\libjpeg\Debug\qjpegd.dll
InputName=qjpegd

"$(TargetDir)\qjpegd.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).dll" "$(TargetDir)\$(TargetName).dll"

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "ANSI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\windows\ansi\fileinfo_win.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\windows\ansi\filethread_win.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\windows\ansi\gotourl_win.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\windows\ansi\wlaunchthread_win.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Unicode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\windows\unicode\fileinfo_win.cpp
# End Source File
# Begin Source File

SOURCE=..\src\windows\unicode\filethread_win.cpp
# End Source File
# Begin Source File

SOURCE=..\src\windows\unicode\gotourl_win.cpp
# End Source File
# Begin Source File

SOURCE=..\src\windows\unicode\wlaunchthread_win.cpp
# End Source File
# End Group
# Begin Group "Unix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\unix\fileinfo_unix.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\unix\filethread_unix.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\unix\gotourl_unix.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\unix\wlaunchthread_unix.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\unix\wlaunchthread_unix.h
# End Source File
# End Group
# End Target
# End Project
