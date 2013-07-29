; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=UserRegisterDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "cassette.h"
LastPage=0

ClassCount=17
Class1=AboutDlg
Class2=AboutView
Class3=AboutWnd
Class4=AccountCateEditingDlg
Class5=AccountCatesDlg
Class6=AccountEditingDlg
Class7=AccountsView
Class8=AccountTypeEdtingDlg
Class9=AccountTypesDlg
Class10=AppSettingsDlg
Class11=CassetteApp
Class12=Dialog
Class13=Label
Class14=MainFrame
Class15=UserLoginDlg
Class16=UserRegisterDlg
Class17=UserSettingsDlg

ResourceCount=16
Resource1=IDD_ABOUTBOX
Resource2=IDD_ACCOUNT_TYPES
Resource3=IDM_OPERATE_ACCOUNTS
Resource4=IDD_USER_LOGIN
Resource5=IDD_USER_SETTINGS
Resource6=IDD_ACCOUNT_TYPE_EDITING
Resource7=IDM_OPERATE_TYPES
Resource8=IDD_USER_REGISTER
Resource9=IDR_MAINFRAME
Resource10=IDD_APP_SETTINGS
Resource11=IDD_ACCOUNT_CATE_EDITING
Resource12=IDM_OPERATE_CATES1
Resource13=IDD_ACCOUNT_CATES
Resource14=IDM_OPERATE_TYPES1
Resource15=IDM_OPERATE_CATES
Resource16=IDD_ACCOUNT_EDITING

[CLS:AboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=AboutDlg.h
ImplementationFile=AboutDlg.cpp
LastObject=AboutDlg

[CLS:AboutView]
Type=0
BaseClass=CHtmlView
HeaderFile=AboutView.h
ImplementationFile=AboutView.cpp
Filter=D
VirtualFilter=7VWC
LastObject=AboutView

[CLS:AboutWnd]
Type=0
BaseClass=CFrameWnd
HeaderFile=AboutWnd.h
ImplementationFile=AboutWnd.cpp
Filter=T
VirtualFilter=fWC
LastObject=AboutWnd

[CLS:AccountCateEditingDlg]
Type=0
BaseClass=Dialog
HeaderFile=AccountCateEditingDlg.h
ImplementationFile=AccountCateEditingDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=AccountCateEditingDlg

[CLS:AccountCatesDlg]
Type=0
BaseClass=Dialog
HeaderFile=AccountCatesDlg.h
ImplementationFile=AccountCatesDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=AccountCatesDlg

[CLS:AccountEditingDlg]
Type=0
BaseClass=Dialog
HeaderFile=AccountEditingDlg.h
ImplementationFile=AccountEditingDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=AccountEditingDlg

[CLS:AccountsView]
Type=0
BaseClass=CListView
HeaderFile=AccountsView.h
ImplementationFile=AccountsView.cpp
LastObject=ID_LIST_STYLE_ICON
Filter=C
VirtualFilter=VWC

[CLS:AccountTypeEdtingDlg]
Type=0
BaseClass=Dialog
HeaderFile=AccountTypeEdtingDlg.h
ImplementationFile=AccountTypeEdtingDlg.cpp
LastObject=IDOK
Filter=D
VirtualFilter=dWC

[CLS:AccountTypesDlg]
Type=0
BaseClass=Dialog
HeaderFile=AccountTypesDlg.h
ImplementationFile=AccountTypesDlg.cpp
LastObject=AccountTypesDlg
Filter=D
VirtualFilter=dWC

[CLS:AppSettingsDlg]
Type=0
BaseClass=Dialog
HeaderFile=AppSettingsDlg.h
ImplementationFile=AppSettingsDlg.cpp
LastObject=AppSettingsDlg
Filter=D
VirtualFilter=dWC

[CLS:CassetteApp]
Type=0
BaseClass=CWinApp
HeaderFile=CassetteApp.h
ImplementationFile=CassetteApp.cpp
LastObject=CassetteApp

[CLS:Dialog]
Type=0
BaseClass=CDialog
HeaderFile=Dialog.h
ImplementationFile=Dialog.cpp
LastObject=Dialog

[CLS:Label]
Type=0
BaseClass=CStatic
HeaderFile=Label.h
ImplementationFile=Label.cpp

[CLS:MainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrame.h
ImplementationFile=MainFrame.cpp
LastObject=ID_ACCOUNT_TYPES
Filter=T
VirtualFilter=fWC

[CLS:UserLoginDlg]
Type=0
BaseClass=Dialog
HeaderFile=UserLoginDlg.h
ImplementationFile=UserLoginDlg.cpp
Filter=D
LastObject=ID_ACCOUNT_CATES
VirtualFilter=dWC

[CLS:UserRegisterDlg]
Type=0
BaseClass=Dialog
HeaderFile=UserRegisterDlg.h
ImplementationFile=UserRegisterDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=ID_ACCOUNT_CATES

[CLS:UserSettingsDlg]
Type=0
BaseClass=Dialog
HeaderFile=UserSettingsDlg.h
ImplementationFile=UserSettingsDlg.cpp
LastObject=UserSettingsDlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=AboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_SYSLINK_WEBSITE,SysLink,1342242816

[DLG:IDD_ACCOUNT_CATES]
Type=1
Class=AccountCatesDlg
ControlCount=1
Control1=IDC_LIST_CATES,SysListView32,1350631425

[DLG:IDD_ACCOUNT_TYPES]
Type=1
Class=AccountTypesDlg
ControlCount=1
Control1=IDC_LIST_TYPES,SysListView32,1350631429

[DLG:IDD_APP_SETTINGS]
Type=1
Class=AppSettingsDlg
ControlCount=12
Control1=IDC_STATIC,button,1342177287
Control2=IDC_CHECK_ENABLEAUTORUN,button,1342242819
Control3=IDC_CHECK_ENABLEHOTKEY,button,1342242819
Control4=IDC_CHECK_ENABLEHTTPSRV,button,1476460547
Control5=IDC_CHECK_ENABLESCHEME,button,1342242819
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_EDIT_DATABASEPATH,edit,1350631552
Control10=IDC_EDIT_BACKUPPATH,edit,1350631552
Control11=IDOK,button,1342242817
Control12=IDCANCEL,button,1342242816

[DLG:IDD_USER_LOGIN]
Type=1
Class=UserLoginDlg
ControlCount=12
Control1=IDC_STATIC,static,1342177283
Control2=IDC_LABEL_LOGIN,static,1342308865
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_USERNAME,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_PASSWORD,edit,1350631584
Control7=IDC_SYSLINK_REGISTER,SysLink,1342242816
Control8=IDC_SYSLINK_FORGETPWD,SysLink,1342242816
Control9=IDC_CHECK_SAVEPWD,button,1342242819
Control10=IDC_CHECK_AUTOLOGIN,button,1342242819
Control11=IDOK,button,1342242817
Control12=IDCANCEL,button,1342242816

[DLG:IDD_USER_REGISTER]
Type=1
Class=UserRegisterDlg
ControlCount=12
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_USERNAME,edit,1350631552
Control7=IDC_EDIT_PASSWORD,edit,1350631584
Control8=IDC_EDIT_CFMPWD,edit,1350631584
Control9=IDC_COMBO_PROTECTLEVEL,combobox,1344339971
Control10=IDC_HOTKEY_CUSTOM,msctls_hotkey32,1350631424
Control11=IDOK,button,1342242817
Control12=IDCANCEL,button,1342242816

[DLG:IDD_USER_SETTINGS]
Type=1
Class=UserSettingsDlg
ControlCount=17
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_USERNAME,edit,1350631552
Control9=IDC_EDIT_OLDPWD,edit,1350631584
Control10=IDC_EDIT_NEWPWD,edit,1350631584
Control11=IDC_EDIT_CFMPWD,edit,1350631584
Control12=IDC_COMBO_PROTECTLEVEL,combobox,1344339971
Control13=IDC_EDIT_CONDONE,edit,1350631552
Control14=IDC_STATIC,button,1342177287
Control15=IDC_HOTKEY_CUSTOM,msctls_hotkey32,1350631424
Control16=IDOK,button,1342242817
Control17=IDCANCEL,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_ADD_ACCOUNT
Command2=ID_MODIFY_ACCOUNT
Command3=ID_DEL_ACCOUNT
Command4=ID_OPEN_URL
Command5=ID_APP_ABOUT
CommandCount=5

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_ADD_ACCOUNT
Command2=ID_MODIFY_ACCOUNT
Command3=ID_DEL_ACCOUNT
Command4=ID_OPEN_URL
Command5=ID_APP_EXIT
Command6=ID_USER_SETTINGS
Command7=ID_APP_SETTINGS
Command8=ID_VIA_AUTOLOGIN
Command9=ID_ACCOUNT_TYPES
Command10=ID_ACCOUNT_CATES
Command11=ID_BACKUP_DATA
Command12=ID_RESUME_DATA
Command13=ID_VIEW_TOOLBAR
Command14=ID_VIEW_STATUS_BAR
Command15=ID_LIST_STYLE_REPORT
Command16=ID_LIST_STYLE_ICON
Command17=ID_HELP
Command18=ID_APP_ABOUT
CommandCount=18

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_MODIFY_ACCOUNT
Command2=ID_OPEN_URL
Command3=ID_ADD_ACCOUNT
Command4=ID_DEL_ACCOUNT
Command5=ID_HELP
Command6=ID_APP_EXIT
CommandCount=6

[MNU:IDM_OPERATE_ACCOUNTS]
Type=1
Class=?
Command1=ID_ADD_ACCOUNT
Command2=ID_MODIFY_ACCOUNT
Command3=ID_DEL_ACCOUNT
Command4=ID_OPEN_URL
CommandCount=4

[MNU:IDM_OPERATE_CATES1]
Type=1
Class=?
Command1=ID_CATE_ADD
Command2=ID_CATE_MODIFY
Command3=ID_CATE_DELETE
CommandCount=3

[MNU:IDM_OPERATE_TYPES1]
Type=1
Class=?
Command1=ID_TYPE_ADD
Command2=ID_TYPE_MODIFY
Command3=ID_TYPE_DELETE
CommandCount=3

[MNU:IDM_OPERATE_TYPES]
Type=1
Class=?
Command1=ID_TYPE_ADD
Command2=ID_TYPE_MODIFY
Command3=ID_TYPE_DELETE
CommandCount=3

[MNU:IDM_OPERATE_CATES]
Type=1
Class=?
Command1=ID_CATE_ADD
Command2=ID_CATE_MODIFY
Command3=ID_CATE_DELETE
CommandCount=3

[DLG:IDD_ACCOUNT_CATE_EDITING]
Type=1
Class=AccountCateEditingDlg
ControlCount=16
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_CATENAME,edit,1350631552
Control8=IDC_EDIT_CATEDESC,edit,1350635716
Control9=IDC_COMBO_TYPES,combobox,1344339971
Control10=IDC_EDIT_URL,edit,1350631552
Control11=IDC_COMBO_STARTUP,combobox,1344339971
Control12=IDC_EDIT_KEYWORDS,edit,1350631552
Control13=IDOK,button,1342242817
Control14=IDCANCEL,button,1342242816
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT_ICOPATH,edit,1350631552

[DLG:IDD_ACCOUNT_TYPE_EDITING]
Type=1
Class=AccountTypeEdtingDlg
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_TYPENAME,edit,1350631552
Control4=IDC_EDIT_SAFERANK,edit,1350631552
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816

[DLG:IDD_ACCOUNT_EDITING]
Type=1
Class=AccountEditingDlg
ControlCount=14
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_MYNAME,edit,1350631552
Control8=IDC_EDIT_ACCOUNTNAME,edit,1350631552
Control9=IDC_EDIT_ACCOUNTPWD,edit,1350631552
Control10=IDC_COMBO_CATES,combobox,1344339971
Control11=IDC_EDIT_SAFERANK,edit,1350631552
Control12=IDC_EDIT_COMMENT,edit,1350635716
Control13=IDOK,button,1342242817
Control14=IDCANCEL,button,1342242816

