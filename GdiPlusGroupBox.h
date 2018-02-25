#pragma once

#pragma warning(disable: 4482)

#ifndef ARGB
	#define ARGB(A, R, G, B) ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B))
#endif

#include "MemDraw.h"

class CGdiPlusGroupBox : public CStatic
{
	DECLARE_DYNAMIC(CGdiPlusGroupBox)

protected:

	CMemDraw	m_MemDraw;
	CRect		m_rcClient;
	bool		m_bNeedRepaint;
	CString		m_strWindowText;
	DWORD		m_dwTextColor;
	int			m_iRoundOffset;
	DWORD		m_dwFrameBorderColor;
	int			m_iFrameBorderWeight;
	DWORD		m_dwTextColorDisabled;
	DWORD		m_dwFrameBorderColorDisabled;
	BOOL		m_bEnabled;

private:

	CString GetText();
	void SetText(CString str);

	DWORD GetTextColor();
	void SetTextColor(DWORD dwValue);

	int GetRoundOffset();
	void SetRoundOffset(int iValue);

	int GetFrameBorderWeight();
	void SetFrameBorderWeight(int iValue);

	DWORD GetTextColorDisabled();
	void SetTextColorDisabled(DWORD dwValue);

	DWORD GetFrameBorderColorDisabled();
	void SetFrameBorderColorDisabled(DWORD dwValue);

public:

	CGdiPlusGroupBox();
	virtual ~CGdiPlusGroupBox();

	Prop<CString, CGdiPlusGroupBox> CG_Text;
	Prop<DWORD, CGdiPlusGroupBox>	CG_TextColor;
	Prop<int, CGdiPlusGroupBox>		CG_RoundOffset;
	Prop<int, CGdiPlusGroupBox>		CG_FrameBorderWeight;
	Prop<DWORD, CGdiPlusGroupBox>	CG_TextColorDisabled;
	Prop<DWORD, CGdiPlusGroupBox>	CG_FrameBorderColorDisabled;

	void UpdateControl();

protected:	// my fucking functions lalalala

	BOOL Redraw(BOOL bUpdateNow);
	static BOOL CALLBACK RedrawChildren(HWND hwnd, LPARAM lParam);

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc);
	virtual void OnDrawText(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc, Gdiplus::Font * pFont, Gdiplus::SizeF & txtSize);
	virtual void OnDrawFrame(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc, Gdiplus::SizeF txtSize);

	virtual Gdiplus::Font * GetGdiPlusDefaultWindowFont(HDC hdc);
	virtual CFont * GetMyFont(HDC hdc);
	virtual Gdiplus::SizeF GetTextSize(	Gdiplus::Graphics * pGraphics, 
										LPCWSTR lpString, 
										Gdiplus::Font * pFont,
										Gdiplus::Rect & rcContainer);

	static BOOL CALLBACK EnableChildren(HWND hwnd, LPARAM lParam);

protected:	// fucking message map, lol, lul, lulz, lolz, trololololo [><]

	DECLARE_MESSAGE_MAP()
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnNcPaint();
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnSize(UINT nType, int cx, int cy);
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnable(WPARAM wParam, LPARAM lParam);
};
