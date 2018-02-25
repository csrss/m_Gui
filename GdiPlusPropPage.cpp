#include "stdafx.h"
#include "GdiPlusPropPage.h"

IMPLEMENT_DYNAMIC(CGdiPlusPropPage, CPropertyPage)

CGdiPlusPropPage::CGdiPlusPropPage(UINT nIDTemplate, UINT nIDCaption, DWORD dwSize)
:	CPropertyPage(nIDTemplate, nIDCaption, dwSize),
	m_bNeedRepaint(true)
{
}

CGdiPlusPropPage::CGdiPlusPropPage(LPCTSTR lpszTemplateName, UINT nIDCaption, DWORD dwSize)
:	CPropertyPage(lpszTemplateName, nIDCaption, dwSize),
	m_bNeedRepaint(true)
{
}

CGdiPlusPropPage::~CGdiPlusPropPage()
{
}

void CGdiPlusPropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGdiPlusPropPage, CPropertyPage)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CGdiPlusPropPage::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CGdiPlusPropPage::Redraw(BOOL bUpdateNow)
{
	m_bNeedRepaint = true;
	if(bUpdateNow)
	{
		return ::RedrawWindow(m_hWnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
	}
	else
	{
		return ::InvalidateRect(m_hWnd, NULL, FALSE);
	}
}

void CGdiPlusPropPage::OnSize(UINT nType, int cx, int cy)
{
	Redraw(TRUE);
	::EnumChildWindows(m_hWnd, UpdateChildren, NULL);
}

BOOL __stdcall CGdiPlusPropPage::UpdateChildren(HWND hWnd, LPARAM lParam)
{
	if(::IsWindow(hWnd) && ::IsWindowVisible(hWnd))
	{
		::PostMessage(hWnd, WM_SIZE, 0, 0);
	}
	return TRUE;
}