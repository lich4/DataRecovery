; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDeepScanDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DeepScan.h"

ClassCount=3
Class1=CDeepScanApp
Class2=CDeepScanDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_DEEPSCAN_DIALOG

[CLS:CDeepScanApp]
Type=0
HeaderFile=DeepScan.h
ImplementationFile=DeepScan.cpp
Filter=N

[CLS:CDeepScanDlg]
Type=0
HeaderFile=DeepScanDlg.h
ImplementationFile=DeepScanDlg.cpp
Filter=D
LastObject=CDeepScanDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=DeepScanDlg.h
ImplementationFile=DeepScanDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DEEPSCAN_DIALOG]
Type=1
Class=CDeepScanDlg
ControlCount=4
Control1=IDC_LIST,SysListView32,1350631428
Control2=IDC_DEEP_SCAN,button,1342242816
Control3=IDC_STOP,button,1342242816
Control4=IDC_SHOW,static,1342308352

