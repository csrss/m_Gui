#pragma once

#include "MemDraw.h"
#include "GdiPlusButton.h"
#include "GdiPlusTabCtrl.h"

#ifndef ARGB
#define ARGB(A, R, G, B) ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B))
#endif

#pragma warning(disable: 4482)

class CGdiPlusPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CGdiPlusPropSheet)

public:

	enum BkgType
	{
		TypeSolidBackground,
		TypeGradientBackground,
		TypeImageBackground,
		TypeVistaGlass,
		TypeDefaultBackground
	};

public:

	CGdiPlusPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CGdiPlusPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CGdiPlusPropSheet();

	void UpdateNow();

	CWnd * GetHelpButton();
	CWnd * GetApplyButton();
	CWnd * GetOkButton();
	CWnd * GetCancelButton();

	DWORD GetSolidBackgroundColor() const;
	void SetSolidBackgroundColor(DWORD dwValue);

	DWORD GetGradientStartColor() const;
	void SetGradientStartColor(DWORD dwValue);

	DWORD GetGradientEndColor() const;
	void SetGradientEndColor(DWORD dwValue);

	bool GetDragable() const;
	void SetDragable(bool bValue);

	int GetGlassFromLeft() const;
	void SetGlassFromLeft(int iValue);

	int GetGlassFromRight() const;
	void SetGlassFromRight(int iValue);

	int GetGlassFromTop() const;
	void SetGlassFromTop(int iValue);

	int GetGlassFromBottom() const;
	void SetGlassFromBottom(int iValue);

	void SetBackgroundImage(int iResourceId, LPCTSTR pType = TEXT("PNG"));

protected:
	DECLARE_MESSAGE_MAP()

	BOOL Redraw(BOOL bUpdateNow);
	static BOOL __stdcall UpdateChildren(HWND hWnd, LPARAM lParam);
	HRESULT DwmIsCompositionEnabled(BOOL *pfEnabled);
	HRESULT DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS * pMarInset);
	void OnMakeRect(Gdiplus::RectF &rc);
	void SetGlassRegion(int nTop, int nBottom, int nRight, int nLeft, bool bUpdateNow = true);
	Gdiplus::Bitmap * ImgFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, int nWidth, int nHeight);
	virtual void OnDrawImageBackground(Gdiplus::Graphics * pGraphics, int nWidth, int nHeight);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType, int cx, int cy);
	void OnPaint();
	LRESULT OnNcHitTest(CPoint point);
	virtual BOOL OnInitDialog();

protected:

	bool			m_bNeedRepaint;
	CMemDraw		m_MemDraw;
	BkgType			m_DlgBackgroundType;
	DWORD			m_dwSolidBackgroundColor;
	DWORD			m_dwGradientStartColor;
	DWORD			m_dwGradientEndColor;
	bool			m_bDragable;
	Gdiplus::Bitmap * m_pCurrentBitmap;
	bool			m_bVistaGlass;
	int				glassLeft, 
					glassRight, 
					glassTop, 
					glassBottom;
	CGdiPlusButton	m_wndOkButton,
					m_wndCancelButton,
					m_wndHelpButton,
					m_wndApplyButton;
	CGdiPlusTabCtrl	m_wndTabControl;
	int				m_iCurrentActivePage;
	int				m_iTabsHeight;
};
