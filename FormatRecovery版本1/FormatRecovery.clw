; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFormatRecoveryDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "FormatRecovery.h"

ClassCount=3
Class1=CFormatRecoveryApp
Class2=CFormatRecoveryDlg

ResourceCount=4
Resource1=IDR_RECOVER
Resource2=IDR_MAINFRAME
Resource3=IDD_SHOWINFO
Class3=CShowInfo
Resource4=IDD_FORMATRECOVERY_DIALOG

[CLS:CFormatRecoveryApp]
Type=0
HeaderFile=FormatRecovery.h
ImplementationFile=FormatRecovery.cpp
Filter=N

[CLS:CFormatRecoveryDlg]
Type=0
HeaderFile=FormatRecoveryDlg.h
ImplementationFile=FormatRecoveryDlg.cpp
Filter=D
LastObject=IDC_SEARCHEXT
BaseClass=CDialogEx
VirtualFilter=dWC

[DLG:IDD_FORMATRECOVERY_DIALOG]
Type=1
Class=CFormatRecoveryDlg
ControlCount=16
Control1=IDC_RESULT,SysListView32,1350631425
Control2=IDC_ALLEXT,SysListView32,1350631699
Control3=IDC_STATIC,static,1342308352
Control4=IDC_ADDEXT,button,1342242816
Control5=IDC_DELEXT,button,1342242816
Control6=IDC_ADDALL,button,1342242816
Control7=IDC_SEARCHEXT,SysListView32,1350631699
Control8=IDC_STATIC,static,1342308352
Control9=IDC_SEARCH,button,1342242816
Control10=IDC_PROGRESS,msctls_progress32,1350565889
Control11=IDC_STATIC,static,1342308352
Control12=IDC_SEARCHAREA,listbox,1352728835
Control13=IDC_STATIC,static,1342308352
Control14=IDC_RECOVERPOS,button,1342242816
Control15=IDC_SAVEPOS,static,1342312448
Control16=IDC_STOP,button,1342242816

[MNU:IDR_RECOVER]
Type=1
Class=CFormatRecoveryDlg
CommandCount=0

[DLG:IDD_SHOWINFO]
Type=1
Class=CShowInfo
ControlCount=1
Control1=IDC_SHOWINFO,edit,1352730628

[CLS:CShowInfo]
Type=0
HeaderFile=ShowInfo.h
ImplementationFile=ShowInfo.cpp
BaseClass=CDialogEx
Filter=D
VirtualFilter=dWC

