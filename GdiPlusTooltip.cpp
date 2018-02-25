// GdiPlusTooltip.cpp : implementation file
//

#include "stdafx.h"
#include "GdiPlusTooltip.h"


// CGdiPlusTooltip

IMPLEMENT_DYNAMIC(CGdiPlusTooltip, CWnd)

CGdiPlusTooltip::CGdiPlusTooltip()
:	m_bNeedRepaint(true),
	m_pCachedBitmap(NULL)
{

}

CGdiPlusTooltip::~CGdiPlusTooltip()
{
	if(m_pCachedBitmap) delete m_pCachedBitmap;
}


BEGIN_MESSAGE_MAP(CGdiPlusTooltip, CWnd)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CGdiPlusTooltip message handlers



HBRUSH CGdiPlusTooltip::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusTooltip::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusTooltip::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&m_rcClient);
}

void CGdiPlusTooltip::OnDestroy()
{
	if(m_pCachedBitmap)
	{
		delete m_pCachedBitmap;
		m_pCachedBitmap = NULL;
	}
}

BOOL CGdiPlusTooltip::Redraw(BOOL bUpdateNow)
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