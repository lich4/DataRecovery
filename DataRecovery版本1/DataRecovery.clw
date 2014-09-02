; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDataRecoveryDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "datarecovery.h"
LastPage=0

ClassCount=7
Class1=CDataRecoveryApp
Class2=CDataRecoveryDlg
Class3=CFindDlg
Class4=Choose
Class5=CListCtrlEx
Class6=CRecoverStatus
Class7=CSplitter

ResourceCount=4
Resource1=IDD_CHOOSE
Resource2=IDD_FIND
Resource3=IDR_MAINMENU
Resource4=IDD_DATARECOVERY_DIALOG

[CLS:CDataRecoveryApp]
Type=0
BaseClass=CWinApp
HeaderFile=DataRecovery.h
ImplementationFile=DataRecovery.cpp
LastObject=ID_FIND

[CLS:CDataRecoveryDlg]
Type=0
BaseClass=CDialog
HeaderFile=DataRecoveryDlg.h
ImplementationFile=DataRecoveryDlg.cpp
LastObject=IDC_BUTTON_END

[CLS:CFindDlg]
Type=0
BaseClass=CDialog
HeaderFile=FindDlg1.h
ImplementationFile=FindDlg1.cpp

[CLS:Choose]
Type=0
BaseClass=CDialog
HeaderFile=hoose.h
ImplementationFile=hoose.cpp

[CLS:CListCtrlEx]
Type=0
BaseClass=CListCtrl
HeaderFile=ListCtrlEx.h
ImplementationFile=ListCtrlEx.cpp

[CLS:CRecoverStatus]
Type=0
BaseClass=CDialog
HeaderFile=RecoverStatus.h
ImplementationFile=RecoverStatus.cpp

[CLS:CSplitter]
Type=0
BaseClass=CButton
HeaderFile=Splitter.h
ImplementationFile=Splitter.cpp

[DLG:IDD_DATARECOVERY_DIALOG]
Type=1
Class=CDataRecoveryDlg
ControlCount=7
Control1=IDC_LIST,SysListView32,1350631689
Control2=IDC_TREE,SysTreeView32,1350631703
Control3=IDC_STATE,static,1342308352
Control4=IDC_BUTTON_END,button,1342242816
Control5=IDC_DELETED,static,1342308352
Control6=IDC_PROGRESS,msctls_progress32,1350565888
Control7=IDC_SPLITTER,button,1342242817

[DLG:IDD_FIND]
Type=1
Class=CFindDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT,edit,1350631552

[DLG:IDD_CHOOSE]
Type=1
Class=Choose
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_TAB,SysTabControl32,1350568000
Control4=IDC_PIC,static,1342177287
Control5=IDREFRESH,button,1342242817

[DLG:IDD_RecoverStatu]
Type=1
Class=CRecoverStatus

[MNU:IDR_MAINMENU]
Type=1
Class=?
Command1=ID_OPEN
Command2=ID_RECOVERY
Command3=ID_FIND
Command4=ID_QUIT
CommandCount=4

