#include "stdafx.h"
#include "GdiPlusDialog.h"


IMPLEMENT_DYNAMIC(CGdiPlusDialog, CDialog)

CGdiPlusDialog::CGdiPlusDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
	: CDialog(lpszTemplateName, pParentWnd), 
	  m_bDragable(true),
	  m_bNeedRepaint(true),
	  m_pCachedBitmap(NULL),
	  m_DlgBackgroundType(TypeDefaultBackground),
	  m_pCurrentBitmap(NULL),
	  m_dwSolidBackgroundColor(MakeArgb(255, 255, 255, 255)),
	  m_bVistaGlass(false),
	  m_dwGradientStartColor(MakeArgb(255, 250, 250, 250)),
	  m_dwGradientEndColor(MakeArgb(255, 217, 217, 217)),
	  glassLeft(0), 
	  glassRight(0), 
	  glassTop(0), 
	  glassBottom(0)
{
	Gdiplus::Status gdiStatus;
	Gdiplus::GdiplusStartupInput in;
	gdiStatus = Gdiplus::GdiplusStartup(&m_gdiToken, &in, NULL);
	if(gdiStatus != Gdiplus::Status::Ok)
	{
		::OutputDebugString(TEXT("Unable to start Gdiplus!\r\n"));
	}
}

CGdiPlusDialog::CGdiPlusDialog(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd), 
	  m_bDragable(true),
	  m_bNeedRepaint(true),
	  m_pCachedBitmap(NULL),
	  m_DlgBackgroundType(TypeDefaultBackground),
	  m_pCurrentBitmap(NULL),
	  m_dwSolidBackgroundColor(MakeArgb(255, 255, 255, 255)),
	  m_bVistaGlass(false),
	  m_dwGradientStartColor(MakeArgb(255, 255, 255, 255)),
	  m_dwGradientEndColor(MakeArgb(255, 255, 255, 255)),
	  glassLeft(0), 
	  glassRight(0), 
	  glassTop(0), 
	  glassBottom(0)
{
	Gdiplus::Status gdiStatus;
	Gdiplus::GdiplusStartupInput in;
	gdiStatus = Gdiplus::GdiplusStartup(&m_gdiToken, &in, NULL);
	if(gdiStatus != Gdiplus::Status::Ok)
	{
		::OutputDebugString(TEXT("Unable to start Gdiplus!\r\n"));
	}
}

HRESULT CGdiPlusDialog::DwmIsCompositionEnabled(BOOL * pfEnabled)
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

HRESULT CGdiPlusDialog::DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS * pMarInset)
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

void CGdiPlusDialog::UpdateNow()
{
	Redraw(TRUE);
}

void CGdiPlusDialog::SetBkgType(BkgType Type)
{
	m_DlgBackgroundType = Type;
}

CGdiPlusDialog::~CGdiPlusDialog()
{
	Gdiplus::GdiplusShutdown(m_gdiToken);
}

void CGdiPlusDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGdiPlusDialog, CDialog)
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CGdiPlusDialog::SetDragable(bool bValue)
{
	m_bDragable = bValue;
}

bool CGdiPlusDialog::GetDragable()
{
	return m_bDragable;
}

DWORD CGdiPlusDialog::MakeArgb(int A, int R, int G, int B)
{
	return ((DWORD(A)<<24) + (DWORD(R)<<16) + (DWORD(G)<<8) + DWORD(B));
}

void CGdiPlusDialog::SetSolidBkgColor(DWORD dwValue, bool bUpdateNow)
{
	m_dwSolidBackgroundColor = dwValue;
	if(bUpdateNow)
	{
		Redraw(TRUE);
	}
}

void CGdiPlusDialog::SetGradientBkgColors(DWORD dwStart, DWORD dwEnd, bool bUpdateNow)
{
	m_dwGradientStartColor = dwStart;
	m_dwGradientEndColor = dwEnd;
	if(bUpdateNow)
	{
		Redraw(TRUE);
	}
}

void CGdiPlusDialog::FillBlackRect(CRect rc, CDC *pDC)
{
	CBrush cbr(RGB(0,0,0));
	pDC->FillRect(&rc, &cbr);
}

void CGdiPlusDialog::FillGlassRegion(CDC *pDC, int nTop, int nBottom, int nRight, int nLeft, bool bUpdateNow)
{
	CRect rcTop(CPoint(0, 0), CSize(Width(), nTop));
	FillBlackRect(rcTop, pDC);

	CRect rcBottom(CPoint(0, Height() - nBottom), CSize(Width(), nBottom));
	FillBlackRect(rcBottom, pDC);

	CRect rcRight(CPoint(Width() - nRight, 0), CSize(nRight, Height()));
	FillBlackRect(rcRight, pDC);

	CRect rcLeft(CPoint(0, 0), CSize(nLeft, Height()));
	FillBlackRect(rcLeft, pDC);
}

int CGdiPlusDialog::Width()
{
	CRect rc;
	GetClientRect(&rc);
	return rc.Width();
}

int CGdiPlusDialog::Height()
{
	CRect rc;
	GetClientRect(&rc);
	return rc.Height();
}

void CGdiPlusDialog::SetGlassRegion(int nTop, int nBottom, int nRight, int nLeft, bool bUpdateNow)
{
	BOOL cEnabled = FALSE;
	HRESULT Result = DwmIsCompositionEnabled(&cEnabled);
	if(Result == S_OK && cEnabled == TRUE)
	{
		m_bVistaGlass = true;

		MARGINS Margins;

		Margins.Bottom = glassBottom = nBottom;
		Margins.Left = glassLeft = nLeft;
		Margins.Right = glassRight = nRight;
		Margins.Top = glassTop = nTop;

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

LRESULT CGdiPlusDialog::OnNcHitTest(CPoint point)
{
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	if(PtInRect(&rcWnd, point) && m_bDragable)
	{
		return HTCAPTION;
	}
	return CDialog::OnNcHitTest(point);
}

void CGdiPlusDialog::OnSize(UINT nType, int cx, int cy)
{
	GetWindowRect(&m_rcClient);
	Redraw(TRUE);
	EnumChildWindows(m_hWnd, EnumChildren, NULL);
}

BOOL CGdiPlusDialog::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusDialog::OnMakeRect(Gdiplus::RectF &rc)
{
	if(m_bVistaGlass)
	{
		rc.X += glassLeft;
		rc.Y += glassTop;
		rc.Width -= glassLeft + glassTop;
		rc.Height -= glassBottom + glassTop;
	}
}

void CGdiPlusDialog::OnDrawImageBackground(Gdiplus::Graphics * pGraphics, int nWidth, int nHeight)
{
	Gdiplus::RectF rc((float)0, (float)0, (float)nWidth, (float)nHeight);
	OnMakeRect(rc);
	if(m_pCurrentBitmap)
	{
		pGraphics->DrawImage(m_pCurrentBitmap, rc);
	}
}

void CGdiPlusDialog::OnDrawContext(Gdiplus::Graphics * pGraphics, int nWidth, int nHeight)
{
	switch(m_DlgBackgroundType)
	{
		case TypeSolidBackground:
			{
				m_MemDraw.GdiSolidFill(m_dwSolidBackgroundColor);
			}
		break;

		case TypeGradientBackground:
			{
				m_MemDraw.GdiGradientFill(NULL, m_dwGradientStartColor, m_dwGradientEndColor);
			}
		break;

		case TypeImageBackground:
			{
				OnDrawImageBackground(pGraphics, nWidth, nHeight);
			}
		break;

		case TypeVistaGlass:
			{
				BOOL bComposite = FALSE;
				if(DwmIsCompositionEnabled(&bComposite) == S_OK && bComposite)
				{
					SetGlassRegion(-1, -1, -1, -1, false);
				}
			}
		break;

		default:
			{

			}
		break;
	}
}

void CGdiPlusDialog::OnDraw(CDC *pDC)
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

void CGdiPlusDialog::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

// update children, redraw them or something
BOOL __stdcall CGdiPlusDialog::EnumChildren(HWND hWnd, LPARAM lParam)
{
	if(::IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SIZE, 0, 0);
	}
	return true;
}

BOOL CGdiPlusDialog::Redraw(BOOL bUpdateNow)
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

void CGdiPlusDialog::SetBackgroundImage(int iResourceId, LPCTSTR pType, bool bUpdateNow)
{
	if(m_pCurrentBitmap) 
	{
		delete m_pCurrentBitmap;
	}
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	m_pCurrentBitmap = ImgFromResource(MAKEINTRESOURCE(iResourceId), pType, hInst);
	if(bUpdateNow)
	{
		Redraw(TRUE);
	}
}

Gdiplus::Bitmap * CGdiPlusDialog::ImgFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
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

LRESULT CGdiPlusDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PRINTCLIENT:
			{
				OnDraw(CDC::FromHandle((HDC)wParam));
			}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CGdiPlusDialog::PreSubclassWindow()
{
	PostMessage(WM_SIZE);
	CDialog::PreSubclassWindow();
}
