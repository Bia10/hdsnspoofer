// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "funcs.h"
#include "MainDlg.h"
	
#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif
	
CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
,mode(0), originSN({0}), newSN({0})
{
	m_bLayoutInited = FALSE;
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
#endif

	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	InitWnd();

	return 0;
}


//TODO:消息映射
void CMainDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}

void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}

void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnMode()
{
	// 快速模式
	if (mode == 0) {
		mode = 1;
		
		SButton* btnMode = FindChildByName2<SButton>(L"btn_mode");
		btnMode->SetWindowText(L"快速模式");
		SWindow* wndAdv = FindChildByName2<SWindow>(L"wnd_adv");
		SWindow* wndRapid = FindChildByName2<SWindow>(L"wnd_rapid");
		wndRapid->SetVisible(FALSE);
		wndAdv->SetVisible(TRUE);
		CRect rct = GetWindowRect();
		SetWindowPos(NULL, 0, 0, 440, rct.Height(), SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOACTIVATE);
	}
	else {
		mode = 0;
		
		SButton* btnMode = FindChildByName2<SButton>(L"btn_mode");
		btnMode->SetWindowText(L"高级模式");
		SWindow* wndAdv = FindChildByName2<SWindow>(L"wnd_adv");
		SWindow* wndRapid = FindChildByName2<SWindow>(L"wnd_rapid");
		wndAdv->SetVisible(FALSE);
		wndRapid->SetVisible(TRUE);
		CRect rct = GetWindowRect();
		SetWindowPos(NULL, 0, 0, 280, rct.Height(), SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOACTIVATE);
	}
}

void CMainDlg::InitWnd()
{
	if (!GetOriginSN(originSN)) {
		SMessageBox(m_hWnd, L"获取硬盘序列号失败!", L"错误", MB_OK);
		OnClose();
	}

	serviceInstalled = IsServiceInstalled();
	
	if (serviceInstalled) {
		SButton* btnInstall = FindChildByName2<SButton>(L"btn_install");
		btnInstall->SetWindowText(L"卸载");
	} 
	else {
		SButton* btnInstall = FindChildByName2<SButton>(L"btn_install");
		btnInstall->SetWindowText(L"安装");
	}
}

void CMainDlg::OnBtnInstall()
{
	if (serviceInstalled) {
		UninstallService();
		SButton* btnInstall = FindChildByName2<SButton>(L"btn_install");
		btnInstall->SetWindowText(L"安装");
		serviceInstalled = FALSE;
	}
	else {
		if (!InstallService()) {
			SMessageBox(m_hWnd, L"驱动安装失败!", L"错误", MB_OK);
			OnClose();
			return;
		}
		else {
			if (!IsDriverRunning()) {
				SMessageBox(m_hWnd, L"驱动安装成功!需要重启计算机.", L"成功", MB_OK);
				RebootOS();
				OnClose();
				return;
			}
		}

		SButton* btnInstall = FindChildByName2<SButton>(L"btn_install");
		btnInstall->SetWindowText(L"卸载");
		serviceInstalled = TRUE;
	}
}

void CMainDlg::OnBtnSpoof()
{
	int cnt = originSN.count;
	GenRandomSN(cnt, newSN);
	if (SpoofHDSN(originSN, newSN)) {
		SMessageBox(m_hWnd, L"修改硬盘序列号成功!", L"完成", MB_OK);
	} else {
		SMessageBox(m_hWnd, L"修改硬盘序列号失败!", L"错误", MB_OK);
	}
}

void CMainDlg::OnBtnApply()
{
	SStringT str;
	TCHAR buf[64];
	for (int i = 0; i < originSN.count; i++)
	{
		str.Format(L"hd%d_new", i);
		SEdit* edit = FindChildByName2<SEdit>(str);
		SStringT text = edit->GetWindowText();
		lstrcpy(buf, text.GetBuffer(0));
		for (int j = 0; j <= text.GetLength(); j++)
		{
			newSN.sn[i][j] = buf[j];
		}
	}
	newSN.count = originSN.count;
	if (SpoofHDSN(originSN, newSN)) {
		SMessageBox(m_hWnd, L"修改硬盘序列号成功!", L"完成", MB_OK);
	}
	else {
		SMessageBox(m_hWnd, L"修改硬盘序列号失败!", L"错误", MB_OK);
	}
}
