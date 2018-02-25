#pragma once

#include <iostream>
#include <vector>

class CDrawingPanel
{
protected:

	struct WINDOWS
	{
		HWND hChildWindow;
		HWND hParentWindow;
	};

	std::vector<WINDOWS> m_vecWindows;

public:

	CDrawingPanel(void);
	CDrawingPanel(HWND hWnd);
	CDrawingPanel(HDC hdc);
	virtual ~CDrawingPanel(void);

	void AssignWindow(HWND hWnd);
	void AssignWindow(HDC hdc);

	void OnSize();
	void AddControl(HWND hWnd);
	void DelControl(HWND hWnd);

protected:

	static BOOL __stdcall EnumChildrenProc(HWND hWnd, LPARAM lParam);

protected:

	HWND	m_hWnd;
	RECT	m_rcClient;
};
