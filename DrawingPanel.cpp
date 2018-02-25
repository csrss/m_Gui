#include "StdAfx.h"
#include "DrawingPanel.h"

CDrawingPanel::CDrawingPanel(void) : m_hWnd(NULL){}
CDrawingPanel::CDrawingPanel(HWND hWnd) : m_hWnd(hWnd){}
CDrawingPanel::CDrawingPanel(HDC hdc) : m_hWnd(::WindowFromDC(hdc)){}
CDrawingPanel::~CDrawingPanel(void){}

void CDrawingPanel::AssignWindow(HWND hWnd)
{
	m_hWnd = hWnd;
}

void CDrawingPanel::AssignWindow(HDC hdc)
{
	m_hWnd = ::WindowFromDC(hdc);
}

void CDrawingPanel::OnSize()
{
	if(m_hWnd)
	{
		::GetClientRect(m_hWnd, &m_rcClient);
	}
}

void CDrawingPanel::AddControl(HWND hWnd)
{
	if(m_hWnd)
	{
		WINDOWS wnds;
		wnds.hChildWindow = hWnd;
		wnds.hParentWindow = ::GetParent(hWnd);

		m_vecWindows.push_back(wnds);

		::SetParent(hWnd, m_hWnd);
	}
}

void CDrawingPanel::DelControl(HWND hWnd)
{
	if(m_hWnd)
	{
		for(std::vector<WINDOWS>::iterator iter = m_vecWindows.begin(); iter != m_vecWindows.end(); ++iter)
		{
			if(iter->hChildWindow == hWnd)
			{
				::SetParent(iter->hChildWindow, iter->hParentWindow);
				m_vecWindows.erase(iter);
				break;
			}
		}
	}
}