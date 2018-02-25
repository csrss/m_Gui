#pragma once

#pragma warning(disable: 4482)
#include "resource.h"
#include "stdafx.h"
#include "MemDraw.h"

class CGdiPlusDialog : public CDialog
{
	typedef struct _MARGINS
	{
		int Left;
		int Right; 
		int Top; 
		int Bottom; 
	} 
	MARGINS; 

	int glassLeft, glassRight, glassTop, glassBottom;

	DECLARE_DYNAMIC(CGdiPlusDialog)

	ULONG_PTR m_gdiToken; 
	bool m_bDragable;
	bool m_bNeedRepaint;
	bool m_bVistaGlass;

	Gdiplus::CachedBitmap * m_pCachedBitmap;
	CRect m_rcClient;
	DWORD m_dwSolidBackgroundColor;
	DWORD m_dwGradientStartColor, m_dwGradientEndColor;
	Gdiplus::Bitmap * m_pCurrentBitmap;

	CMemDraw	m_MemDraw;

public:

	enum BkgType
	{
		TypeSolidBackground,
		TypeGradientBackground,
		TypeImageBackground,
		TypeVistaGlass,
		TypeDefaultBackground
	};

	BkgType m_DlgBackgroundType;

	CGdiPlusDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CGdiPlusDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CGdiPlusDialog();

	void SetBkgType(BkgType Type = TypeDefaultBackground);
	void SetSolidBkgColor(DWORD dwValue, bool bUpdateNow = true);
	void SetGradientBkgColors(DWORD dwStart, DWORD dwEnd, bool bUpdateNow = true);
	void SetGlassRegion(int nTop, int nBottom, int nRight, int nLeft, bool bUpdateNow = true);
	void SetBackgroundImage(int iResourceId, LPCTSTR pType, bool bUpdateNow = true);
	void FillGlassRegion(CDC *pDC, int nTop, int nBottom, int nRight, int nLeft, bool bUpdateNow = true);

	DWORD MakeArgb(int A, int R, int G, int B);
	void UpdateNow();

protected:

	void SetDragable(bool bValue);
	bool GetDragable();

	virtual void OnMakeRect(Gdiplus::RectF &rc);
	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, int nWidth, int nHeight);
	virtual void OnDrawImageBackground(Gdiplus::Graphics * pGraphics, int nWidth, int nHeight);

	int Width();
	int Height();
	void FillBlackRect(CRect rc, CDC *pDC);
	HRESULT DwmIsCompositionEnabled(BOOL *pfEnabled);
	HRESULT DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS * pMarInset);
	Gdiplus::Bitmap * ImgFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);
	BOOL Redraw(BOOL bUpdateNow);
	static BOOL __stdcall EnumChildren(HWND hWnd, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	LRESULT OnNcHitTest(CPoint point);
	void OnSize(UINT nType, int cx, int cy);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
};
