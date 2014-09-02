; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFindStringDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "FindString.h"

ClassCount=3
Class1=CFindStringApp
Class2=CFindStringDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_FINDSTRING_DIALOG

[CLS:CFindStringApp]
Type=0
HeaderFile=FindString.h
ImplementationFile=FindString.cpp
Filter=N

[CLS:CFindStringDlg]
Type=0
HeaderFile=FindStringDlg.h
ImplementationFile=FindStringDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CFindStringDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=FindStringDlg.h
ImplementationFile=FindStringDlg.cpp
Filter=D
LastObject=IDOK

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_FINDSTRING_DIALOG]
Type=1
Class=CFindStringDlg
ControlCount=9
Control1=IDC_DRIVE,SysListView32,1350631427
Control2=IDC_STOP,button,1342242816
Control3=IDC_SCAN,button,1342242816
Control4=IDC_KEYWORD,SysListView32,1350631427
Control5=IDC_STATIC,static,1342308352
Control6=IDC_RESULT,SysListView32,1350631427
Control7=IDC_STATIC,static,1342308352
Control8=IDC_DATA,edit,1350631552
Control9=IDC_SHOW,static,1342308352

