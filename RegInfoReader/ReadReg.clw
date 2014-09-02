; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CReadRegDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ReadReg.h"

ClassCount=3
Class1=CReadRegApp
Class2=CReadRegDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_READREG_DIALOG
Resource4=IDR_MENU

[CLS:CReadRegApp]
Type=0
HeaderFile=ReadReg.h
ImplementationFile=ReadReg.cpp
Filter=N

[CLS:CReadRegDlg]
Type=0
HeaderFile=ReadRegDlg.h
ImplementationFile=ReadRegDlg.cpp
Filter=D
LastObject=IDC_LIST
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=ReadRegDlg.h
ImplementationFile=ReadRegDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_READREG_DIALOG]
Type=1
Class=CReadRegDlg
ControlCount=2
Control1=IDC_TREE,SysTreeView32,1350631431
Control2=IDC_LIST,SysListView32,1350631424

[MNU:IDR_MENU]
Type=1
Class=?
Command1=ID_OPEN
CommandCount=1

