#pragma once


// CGdiPlusTooltip

class CGdiPlusTooltip : public CWnd
{
	DECLARE_DYNAMIC(CGdiPlusTooltip)

	bool m_bNeedRepaint;
	Gdiplus::CachedBitmap * m_pCachedBitmap;
	CRect m_rcClient;

public:
	CGdiPlusTooltip();
	virtual ~CGdiPlusTooltip();

protected:

	BOOL Redraw(BOOL bUpdateNow);

	DECLARE_MESSAGE_MAP()

	virtual void OnDraw(CDC *pDC);
	
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType, int cx, int cy);
	void OnDestroy();
};


