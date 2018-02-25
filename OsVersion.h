#pragma once

class COsVersion
{
public:
	COsVersion(void);
	virtual ~COsVersion(void);

	bool Supported() const;

private:

	bool	m_bSupported;
};
