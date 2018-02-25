#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <Gdiplus.h>
#include <afxcmn.h>
#include <afxdlgs.h>

template<typename T, typename T2> class Prop
{
private:
	T (T2::*Get)();
	void (T2::*Set)(T Val);
	T2 *Class;
public:
	Prop() : Get(NULL), Set(NULL), Class(NULL){}
	Prop(T2 *ClassA, T (T2::*GetA)(), void (T2::*SetA)(T Val))
		: Class(ClassA), Get(GetA), Set(SetA){}
	void Set_Prop(T2 *ClassA, T (T2::*GetA)(), void (T2::*SetA)(T Val))
	{
		Class = ClassA, Get = GetA, Set = SetA;
	}
	void Set_Get(T (T2::*GetA)())
	{
		Get = GetA;
	}
	void Set_Set(void (T2::*SetA)(T Val))
	{
		Set = SetA;
	}
	void Set_Class(T2 *cl)
	{
		Class = cl;
	}
	T operator = (const T& In)
	{
		if(Set)
			(Class->*Set)(In);
		return In;
	}
	T operator = (Prop& In)
	{
		if(Set)
			(Class->*Set)(In);
		return In;
	}
	operator T()
	{
		return (Get) ? (Class->*Get)() : 0;
	}
};
