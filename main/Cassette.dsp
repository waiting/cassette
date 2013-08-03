# Microsoft Developer Studio Project File - Name="Cassette" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Cassette - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cassette.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cassette.mak" CFG="Cassette - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cassette - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Cassette - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cassette - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../bin/release"
# PROP BASE Intermediate_Dir "../bin/release/objs"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../bin/release"
# PROP Intermediate_Dir "../bin/release/objs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "NOT_EXIST_REGOPERATE_DLL" /D "SQLITE_HAS_CODEC" /YX"Cassette.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 sqlite3secure.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Cassette - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../bin/debug"
# PROP BASE Intermediate_Dir "../bin/debug/objs"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../bin/debug"
# PROP Intermediate_Dir "../bin/debug/objs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "NOT_EXIST_REGOPERATE_DLL" /D "SQLITE_HAS_CODEC" /FR /YX"Cassette.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sqlite3secure_d.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Cassette - Win32 Release"
# Name "Cassette - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutView.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountCateEditingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountCatesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountEditingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountsView.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountTypeEditingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AccountTypesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AppSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Cassette.cpp
# End Source File
# Begin Source File

SOURCE=.\Cassette.rc
# End Source File
# Begin Source File

SOURCE=.\CassetteApp.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ForgetPwdStep1Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Functional.cpp
# End Source File
# Begin Source File

SOURCE=.\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\UserLoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UserRegisterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UserSettingsDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutView.h
# End Source File
# Begin Source File

SOURCE=.\AboutWnd.h
# End Source File
# Begin Source File

SOURCE=.\AccountCateEditingDlg.h
# End Source File
# Begin Source File

SOURCE=.\AccountCatesDlg.h
# End Source File
# Begin Source File

SOURCE=.\AccountEditingDlg.h
# End Source File
# Begin Source File

SOURCE=.\AccountsView.h
# End Source File
# Begin Source File

SOURCE=.\AccountTypeEditingDlg.h
# End Source File
# Begin Source File

SOURCE=.\AccountTypesDlg.h
# End Source File
# Begin Source File

SOURCE=.\AppSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Cassette.h
# End Source File
# Begin Source File

SOURCE=.\CassetteApp.h
# End Source File
# Begin Source File

SOURCE=.\Dialog.h
# End Source File
# Begin Source File

SOURCE=.\ForgetPwdStep1Dlg.h
# End Source File
# Begin Source File

SOURCE=.\Functional.h
# End Source File
# Begin Source File

SOURCE=.\Label.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\UserLoginDlg.h
# End Source File
# Begin Source File

SOURCE=.\UserRegisterDlg.h
# End Source File
# Begin Source File

SOURCE=.\UserSettingsDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\res\AccountMaster_01.ico
# End Source File
# Begin Source File

SOURCE=..\res\AccountMaster_02.ico
# End Source File
# Begin Source File

SOURCE=..\res\AccountMaster_03.ico
# End Source File
# Begin Source File

SOURCE=.\res\Cassette.ico
# End Source File
# Begin Source File

SOURCE=..\res\Cassette.ico
# End Source File
# Begin Source File

SOURCE=.\res\Cassette.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar1.bmp
# End Source File
# End Group
# Begin Group "winplus"

# PROP Default_Filter "c;cpp;h;hpp"
# Begin Source File

SOURCE=..\..\winplus\src\compoundfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\compoundfile.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\console.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\console.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\definitions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\definitions.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\graphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\graphics.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\ini.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\ini.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\md5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\md5.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\picture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\picture.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\resource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\resource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\shell.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\shell.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\strings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\strings.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\system.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\system.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\time.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\time.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\winctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\winctrl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\winplus.hpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\src\wordslib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winplus\include\wordslib.hpp
# End Source File
# End Group
# Begin Group "registrylib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\MyCore\RegOperate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\MyCore\RegOperate.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\res\about.html
# End Source File
# Begin Source File

SOURCE=..\res\AccountMaster.manifest
# End Source File
# Begin Source File

SOURCE=..\res\AccountMaster_01.png
# End Source File
# Begin Source File

SOURCE=..\res\db.key
# End Source File
# Begin Source File

SOURCE=..\res\dbschema.sql
# End Source File
# Begin Source File

SOURCE=..\res\Toolbar.png
# End Source File
# End Target
# End Project
