#include "StdAfx.h"
#include "OsVersion.h"

COsVersion::COsVersion(void) : m_bSupported(true)
{
	OSVERSIONINFO OSversion;
	OSversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&OSversion);
	if(OSversion.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		m_bSupported = false;
		return;
	}
	if(OSversion.dwMajorVersion < 6)
	{
		m_bSupported = false;
		return;
	}
}

COsVersion::~COsVersion(void)
{
}

bool COsVersion::Supported() const
{
	return m_bSupported;
}