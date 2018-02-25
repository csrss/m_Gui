#include "stdafx.h"
#include "GdiPlusPropSheet.h"

IMPLEMENT_DYNAMIC(CGdiPlusPropSheet, CPropertySheet)

CGdiPlusPropSheet::CGdiPlusPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
:	CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
	m_bNeedRepaint(true),
	m_dwSolidBackgroundColor(RGB(255, 255, 255)),
	m_DlgBackgroundType(BkgType::TypeVistaGlass),
	m_dwGradientStartColor(RGB(250, 250, 250)),
	m_dwGradientEndColor(RGB(217, 217, 217)),
	m_bDragable(true),
	m_bVistaGlass(false),
	m_iCurrentActivePage(0),
	m_iTabsHeight(50)
{
}

CGdiPlusPropSheet::CGdiPlusPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
:	CPropertySheet(pszCaption, pParentWnd, iSelectPage),
	m_bNeedRepaint(true),
	m_dwSolidBackgroundColor(RGB(255, 255, 255)),
	m_DlgBackgroundType(BkgType::TypeVistaGlass),
	m_dwGradientStartColor(RGB(250, 250, 250)),
	m_dwGradientEndColor(RGB(217, 217, 217)),
	m_bDragable(true),
	m_bVistaGlass(false),
	m_iCurrentActivePage(0),
	m_iTabsHeight(50)
{
}

CGdiPlusPropSheet::~CGdiPlusPropSheet()
{
}

BEGIN_MESSAGE_MAP(CGdiPlusPropSheet, CPropertySheet)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CGdiPlusPropSheet::Redraw(BOOL bUpdateNow)
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

BOOL __stdcall CGdiPlusPropSheet::UpdateChildren(HWND hWnd, LPARAM lParam)
{
	if(::IsWindow(hWnd) && ::IsWindowVisible(hWnd))
	{
		::PostMessage(hWnd, WM_SIZE, 0, 0);
	}
	return TRUE;
}

BOOL CGdiPlusPropSheet::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusPropSheet::OnSize(UINT nType, int cx, int cy)
{
	Redraw(TRUE);
	::EnumChildWindows(m_hWnd, UpdateChildren, NULL);
}

void CGdiPlusPropSheet::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

LRESULT CGdiPlusPropSheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PRINTCLIENT:
		{
			OnDraw(CDC::FromHandle((HDC)wParam));
		}
		break;

		case WM_COMMAND:
		{
			GetParent()->PostMessage(WM_COMMAND, wParam, lParam);
		}
		break;
	}
	return CPropertySheet::WindowProc(message, wParam, lParam);
}

void CGdiPlusPropSheet::OnDraw(CDC *pDC)
{
	if(m_bNeedRepaint)
	{
		m_MemDraw.Assign(pDC->GetSafeHdc());
		Gdiplus::Graphics * pGraphics = Gdiplus::Graphics::FromHDC(m_MemDraw);
		OnDrawContext(pGraphics, m_MemDraw.Width(), m_MemDraw.Height());
		if(pGraphics) delete pGraphics;
		m_bNeedRepaint = false;
	}
	m_MemDraw.Commit(pDC->GetSafeHdc());
}

void CGdiPlusPropSheet::OnDrawContext(Gdiplus::Graphics * pGraphics, int nWidth, int nHeight)
{
	switch(m_DlgBackgroundType)
	{
		case BkgType::TypeSolidBackground:
		{
			m_MemDraw.GdiSolidFill(m_dwSolidBackgroundColor);
		}
		break;

		case BkgType::TypeGradientBackground:
		{
			m_MemDraw.GdiGradientFill(NULL, m_dwGradientStartColor, m_dwGradientEndColor);
		}
		break;

		case BkgType::TypeImageBackground:
		{
			OnDrawImageBackground(pGraphics, nWidth, nHeight);
		}
		break;

		case BkgType::TypeVistaGlass:
		{
			BOOL bComposite = FALSE;
			if(DwmIsCompositionEnabled(&bComposite) == S_OK && bComposite)
			{
				SetGlassRegion(-1, -1, -1, -1, false);
			}
		}
		break;
	}
}

LRESULT CGdiPlusPropSheet::OnNcHitTest(CPoint point)
{
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	if(PtInRect(&rcWnd, point) && m_bDragable)
	{
		return HTCAPTION;
	}
	return CPropertySheet::OnNcHitTest(point);
}

HRESULT CGdiPlusPropSheet::DwmIsCompositionEnabled(BOOL *pfEnabled)
{
	HRESULT (__stdcall * _DwmIsCompositionEnabled)(BOOL *pfEnabled) = 0;
	_DwmIsCompositionEnabled = (HRESULT (__stdcall *)(BOOL *pfEnabled))
		::GetProcAddress(::LoadLibrary(TEXT("dwmapi.dll")), "DwmIsCompositionEnabled");
	if(_DwmIsCompositionEnabled)
	{
		return _DwmIsCompositionEnabled(pfEnabled);
	}
	::OutputDebugString(TEXT("DwmIsCompositionEnabled not found.\r\n"));
	return -1;
}

HRESULT CGdiPlusPropSheet::DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS * pMarInset)
{
	HRESULT (__stdcall * _DwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS * pMarInset) = 0;
	_DwmExtendFrameIntoClientArea = 
		(HRESULT (__stdcall *)(HWND hWnd, const MARGINS * pMarInset))
		::GetProcAddress(::LoadLibrary(TEXT("dwmapi.dll")), "DwmExtendFrameIntoClientArea");
	if(_DwmExtendFrameIntoClientArea)
	{
		return _DwmExtendFrameIntoClientArea(hWnd, pMarInset);
	}
	::OutputDebugString(TEXT("DwmExtendFrameIntoClientArea not found.\r\n"));
	return -1;
}

void CGdiPlusPropSheet::OnDrawImageBackground(Gdiplus::Graphics * pGraphics, int nWidth, int nHeight)
{
	Gdiplus::RectF rc((float)0, (float)0, (float)nWidth, (float)nHeight);
	OnMakeRect(rc);
	if(m_pCurrentBitmap)
	{
		pGraphics->DrawImage(m_pCurrentBitmap, rc);
	}
}

void CGdiPlusPropSheet::OnMakeRect(Gdiplus::RectF &rc)
{
	if(m_bVistaGlass)
	{
		rc.X += glassLeft;
		rc.Y += glassTop;
		rc.Width -= glassLeft + glassTop;
		rc.Height -= glassBottom + glassTop;
	}
}

void CGdiPlusPropSheet::SetGlassRegion(int nTop, int nBottom, int nRight, int nLeft, bool bUpdateNow)
{
	BOOL cEnabled = FALSE;
	HRESULT Result = DwmIsCompositionEnabled(&cEnabled);
	if(Result == S_OK && cEnabled == TRUE)
	{
		m_bVistaGlass = true;

		MARGINS Margins;

		Margins.cyBottomHeight = glassBottom = nBottom;
		Margins.cxLeftWidth = glassLeft = nLeft;
		Margins.cxRightWidth = glassRight = nRight;
		Margins.cyTopHeight = glassTop = nTop;

		Result = DwmExtendFrameIntoClientArea(this->m_hWnd, &Margins);
		if(Result != S_OK)
		{
			::OutputDebugString(TEXT("Cannnot implement glass efect!\r\n"));
		}

		if(bUpdateNow)
		{
			Redraw(TRUE);
		}
	}
	else
	{
		m_bVistaGlass = false;
		::OutputDebugString(TEXT("This is not vista or glass is not enabled!\r\n"));
	}
}

Gdiplus::Bitmap * CGdiPlusPropSheet::ImgFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
	Gdiplus::Bitmap * m_pBitmap;
	HGLOBAL m_hBuffer;
	HRSRC hResource = FindResource(hInst, pName, pType);
	if (!hResource) 
	{
		return NULL;
	}
	DWORD imageSize = SizeofResource(hInst, hResource);
	if (!imageSize) 
	{
		return NULL;
	}
	const void * pResourceData = LockResource(LoadResource(hInst, hResource));
	if (!pResourceData) 
	{
		return NULL;
	}
	m_hBuffer  = GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (m_hBuffer)
	{
		void* pBuffer = GlobalLock(m_hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);
			IStream* pStream = NULL;
			if (CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
				if (m_pBitmap)
				{
					if (m_pBitmap->GetLastStatus() == Gdiplus::Ok)
					{
						return m_pBitmap;
					} 
					else 
					{
						delete m_pBitmap;
						m_pBitmap = NULL;
					}
				}
			} 
			else 
			{
				GlobalUnlock(m_hBuffer);
			}
		} 
		else 
		{
			GlobalFree(m_hBuffer);
			m_hBuffer = NULL;
		}
	} 
	return NULL;
}

void CGdiPlusPropSheet::UpdateNow()
{
	Redraw(TRUE);
}

CWnd * CGdiPlusPropSheet::GetHelpButton()
{
	return GetDlgItem(IDHELP);
}

CWnd * CGdiPlusPropSheet::GetApplyButton()
{
	return GetDlgItem(ID_APPLY_NOW);
}

CWnd * CGdiPlusPropSheet::GetOkButton()
{
	return GetDlgItem(IDOK);
}

CWnd * CGdiPlusPropSheet::GetCancelButton()
{
	return GetDlgItem(IDCANCEL);
}

DWORD CGdiPlusPropSheet::GetSolidBackgroundColor() const
{
	return m_dwSolidBackgroundColor;
}

void CGdiPlusPropSheet::SetSolidBackgroundColor(DWORD dwValue)
{
	m_dwSolidBackgroundColor = dwValue;
	m_hWnd ? Redraw(TRUE) : 0;
}

DWORD CGdiPlusPropSheet::GetGradientStartColor() const
{
	return m_dwGradientStartColor;
}

void CGdiPlusPropSheet::SetGradientStartColor(DWORD dwValue)
{
	m_dwGradientStartColor = dwValue;
	m_hWnd ? Redraw(TRUE) : 0;
}

DWORD CGdiPlusPropSheet::GetGradientEndColor() const
{
	return m_dwGradientEndColor;
}

void CGdiPlusPropSheet::SetGradientEndColor(DWORD dwValue)
{
	m_dwGradientEndColor = dwValue;
	m_hWnd ? Redraw(TRUE) : 0;
}

bool CGdiPlusPropSheet::GetDragable() const
{
	return m_bDragable;
}

void CGdiPlusPropSheet::SetDragable(bool bValue)
{
	m_bDragable = bValue;
	m_hWnd ? Redraw(TRUE) : 0;
}

int CGdiPlusPropSheet::GetGlassFromLeft() const
{
	return glassLeft;
}

void CGdiPlusPropSheet::SetGlassFromLeft(int iValue)
{
	glassLeft = iValue;
	m_hWnd ? Redraw(TRUE) : 0;
}

int CGdiPlusPropSheet::GetGlassFromRight() const
{
	return glassRight;
}

void CGdiPlusPropSheet::SetGlassFromRight(int iValue)
{
	glassRight = iValue;
	m_hWnd ? Redraw(TRUE) : 0;
}

int CGdiPlusPropSheet::GetGlassFromTop() const
{
	return glassTop;
}

void CGdiPlusPropSheet::SetGlassFromTop(int iValue)
{
	glassTop = iValue;
	m_hWnd ? Redraw(TRUE) : 0;
}

int CGdiPlusPropSheet::GetGlassFromBottom() const
{
	return glassBottom;
}

void CGdiPlusPropSheet::SetGlassFromBottom(int iValue)
{
	glassBottom = iValue;
	m_hWnd ? Redraw(TRUE) : 0;
}

void CGdiPlusPropSheet::SetBackgroundImage(int iResourceId, LPCTSTR pType)
{
	if(m_pCurrentBitmap) 
	{
		delete m_pCurrentBitmap;
	}
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	m_pCurrentBitmap = ImgFromResource(MAKEINTRESOURCE(iResourceId), pType, hInst);
	m_hWnd ? Redraw(TRUE) : 0;
}

BOOL CGdiPlusPropSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	if(!m_wndOkButton.SubclassWindow(GetOkButton()->GetSafeHwnd()))
	{
		return FALSE;
	}
	if(!m_wndCancelButton.SubclassWindow(GetCancelButton()->GetSafeHwnd()))
	{
		return FALSE;
	}
	if(!m_wndHelpButton.SubclassWindow(GetHelpButton()->GetSafeHwnd()))
	{
		return FALSE;
	}
	if(!m_wndApplyButton.SubclassWindow(GetApplyButton()->GetSafeHwnd()))
	{
		return FALSE;
	}
	m_wndTabControl.SubclassWindow(GetTabControl()->GetSafeHwnd());
	m_wndTabControl.SetItemSize(CSize(m_iTabsHeight, m_iTabsHeight));
	m_wndTabControl.SetCurSel(m_iCurrentActivePage);
	m_wndTabControl.SetCurFocus(m_iCurrentActivePage);
	this->SetActivePage(m_iCurrentActivePage);
	return bResult;
}
