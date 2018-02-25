#pragma once

#ifndef ARGB
#define ARGB(A, R, G, B) ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B))
#endif

class CGdiPlusPropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGdiPlusPropPage)

public:
	CGdiPlusPropPage(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	CGdiPlusPropPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	virtual ~CGdiPlusPropPage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	BOOL Redraw(BOOL bUpdateNow);
	static BOOL __stdcall UpdateChildren(HWND hWnd, LPARAM lParam);

protected:

	bool	m_bNeedRepaint;
};
