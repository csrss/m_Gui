#pragma once

#include "MemDraw.h"

class CGdiPlusDraw : public CMemDraw
{

public:		// methods

	CGdiPlusDraw(HDC hDC = NULL);
	virtual ~CGdiPlusDraw(void);

	operator Gdiplus::Graphics *();

	void InitGraphics();
	Gdiplus::Graphics * Graphics();
	Gdiplus::Font * GetGdiPlusFont();
	Gdiplus::RectF GetGdiPlusRect();

protected:	// methods

private:	// methods

public:		// variables

protected:	// variables

	Gdiplus::Graphics * m_pGraphics;

private:	// variables
};
