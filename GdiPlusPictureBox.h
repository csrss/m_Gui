#pragma once

#include "MemDraw.h"

#ifndef ARGB
	#define ARGB(A, R, G, B) ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B))
#endif

class CGdiPlusPictureBox : public CStatic
{
	DECLARE_DYNAMIC(CGdiPlusPictureBox)

protected:

	CMemDraw		m_MemDraw;
	bool			m_bNeedRepaint;
	BOOL			m_bBorder;
	BOOL			m_bDoubleBorder;
	DWORD			m_dwBorderColor;
	DWORD			m_dwDoubleBorderColor;
	Gdiplus::Image	* m_pImage;

private:

	void SetBorder(BOOL bValue);
	BOOL GetBorder();
	void SetDoubleBorder(BOOL bValue);
	BOOL GetDoubleBorder();
	void SetBorderColor(DWORD dwValue);
	DWORD GetBorderColor();
	void SetDoubleBorderColor(DWORD dwValue);
	DWORD GetDoubleBorderColor();

public:
	CGdiPlusPictureBox();
	virtual ~CGdiPlusPictureBox();

	void SetImage(LPCWSTR lpPath);
	void SetImage(UINT iResId, LPTSTR pType, HMODULE hMod);

	Prop<BOOL, CGdiPlusPictureBox>	CG_Border;
	Prop<BOOL, CGdiPlusPictureBox>	CG_DoubleBorder;
	Prop<DWORD, CGdiPlusPictureBox>	CG_BorderColor;
	Prop<DWORD, CGdiPlusPictureBox>	CG_DoubleBorderColor;

	void UpdateControl();

protected:

	void Redraw(BOOL bUpdateNow);
	Gdiplus::Bitmap *ImgFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);
	Gdiplus::Bitmap *ImgFromPath(LPCWSTR lpPathString);

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc);
	virtual void OnDrawBorder(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc);

	DECLARE_MESSAGE_MAP()
	
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnNcPaint();
	void OnPaint();
	void OnSize(UINT nType, int cx, int cy);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
