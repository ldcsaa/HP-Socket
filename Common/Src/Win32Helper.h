/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.7
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

#include "GeneralHelper.h"

// RECT 帮助宏
#define RC_WIDTH(rc)		(rc.right - rc.left)
#define RC_HEIGHT(rc)		(rc.bottom - rc.top)
#define RC_CENTER_X(rc)		((rc.right + rc.left) / 2)
#define RC_CENTER_Y(rc)		((rc.bottom + rc.top) / 2)

/************************************************************************/
/*                 			  消息映射帮助宏			 					*/
/************************************************************************/

/* see: WindowsX.h */
#define HANDLE_SYS_MSG(hwnd, message, fn)	HANDLE_MSG(hwnd, message, fn)

/* LRESULT Cls_OnMessage(HWND hwnd, WPARAM wParam, LPARAM lParam) */
#define HANDLE_USER_MSG(hwnd, message, fn)								\
	case (message): return (LRESULT)(fn)((hwnd), (wParam), (lParam))

#define FORWARD_USER_MSG(hwnd, message, wParam, lParam, fn)				\
	(LRESULT)(fn)((hwnd), (message), (wParam), (lParam))

#define GET_WND_PROC_INTERNAL(theClass, flag)	((WNDPROC)theClass##flag##WndProc)
#define GET_DLG_PROC_INTERNAL(theClass, flag)	((DLGPROC)theClass##flag##DlgProc)

#define DECLARE_MSG_MAP_INTERNAL(theClass, flag)		\
	static LRESULT CALLBACK theClass##flag##WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define DECLARE_DLG_MSG_MAP_INTERNAL(theClass, flag)	\
	static BOOL CALLBACK theClass##flag##DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define BEGIN_MSG_MAP_INTERNAL(theClass, flag)			\
	LRESULT theClass##flag##WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)	\
	{																					\
		LRESULT result = 0;																\
																						\
		switch(msg)																		\
		{

#define BEGIN_DLG_MSG_MAP_INTERNAL(theClass, flag)		\
	BOOL theClass##flag##DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)		\
	{																					\
		BOOL	retVal = TRUE;															\
		LRESULT result = 0;																\
																						\
		switch(msg)																		\
		{

// 窗口过程为类中的静态成员函数
#define GET_WND_PROC(theClass)			GET_WND_PROC_INTERNAL(theClass, ::)
#define GET_DLG_PROC(theClass)			GET_DLG_PROC_INTERNAL(theClass, ::)

#define DECLARE_MSG_MAP(theClass)					\
public:												\
	DECLARE_MSG_MAP_INTERNAL(theClass, ::)

#define DECLARE_DLG_MSG_MAP(theClass)				\
public:												\
	DECLARE_DLG_MSG_MAP_INTERNAL(theClass, ::)

#define BEGIN_MSG_MAP(theClass)			BEGIN_MSG_MAP_INTERNAL(theClass, ::)
#define BEGIN_DLG_MSG_MAP(theClass)		BEGIN_DLG_MSG_MAP_INTERNAL(theClass, ::)

/* 消息处理函数的声明请参考: <WindowsX.h> 的 HANDLE_MSG */
#define ADD_MSG_MAP(msg, fn)						\
		case (msg): result = HANDLE_##msg((hWnd), (wParam), (lParam), (fn));	break;

/* LRESULT Cls_OnMessage(HWND hwnd, WPARAM wParam, LPARAM lParam) */
#define ADD_USER_MSG_MAP(msg, fn)					\
		case (msg): result = (LRESULT)(fn)((hWnd), (wParam), (lParam));			break;

#define END_MSG_MAP()								\
		default:									\
			result = ::DefWindowProc(hWnd, msg, wParam, lParam);						\
		}																				\
																						\
		return result;																	\
	}

#define END_DLG_MSG_MAP()							\
		default:									\
			retVal = FALSE;																\
		}																				\
																						\
		if(retVal)																		\
			SetDlgMsgResult(hWnd, msg, result);											\
																						\
		return retVal;																	\
	}

// 窗口过程为全局函数
#define GET_GLOBAL_WND_PROC(theClass)			GET_WND_PROC_INTERNAL(theClass,			_)
#define DECLARE_GLOBAL_MSG_MAP(theClass)		DECLARE_MSG_MAP_INTERNAL(theClass,		_)
#define BEGIN_GLOBAL_MSG_MAP(theClass)			BEGIN_MSG_MAP_INTERNAL(theClass,		_)
#define END_GLOBAL_MSG_MAP()					END_MSG_MAP()

#define GET_GLOBAL_DLG_PROC(theClass)			GET_DLG_PROC_INTERNAL(theClass,			_)
#define DECLARE_GLOBAL_DLG_MSG_MAP(theClass)	DECLARE_DLG_MSG_MAP_INTERNAL(theClass,	_)
#define BEGIN_GLOBAL_DLG_MSG_MAP(theClass)		BEGIN_DLG_MSG_MAP_INTERNAL(theClass,	_)
#define END_GLOBAL_DLG_MSG_MAP()				END_DLG_MSG_MAP()

// 绑定对象指针到窗口
#define ATTACH_OBJ_PTR_TO_WINDOW(hwnd, objPtr)	::SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)objPtr)
#define GET_OBJ_PTR_FROM_WINDOW(hwnd, theClass)	(theClass*)(LONG_PTR)::GetWindowLongPtr(hwnd, GWL_USERDATA)

#define DEFINE_OBJ_PTR_FROM_WINDOW(hwnd, theClass, pObj)		\
	theClass* pObj = GET_OBJ_PTR_FROM_WINDOW(hwnd, theClass);	\
	ASSERT(pObj);


/************************************************************************/
/*                 		  应用程序唯一实例			 	            */
/************************************************************************/

class COnlyOneApp
{
public:
	BOOL IsFirstApp() {return m_bIsFirstApp;}

	COnlyOneApp(LPCTSTR pszAppFlag)
	: m_bIsFirstApp(FALSE)
	{
		m_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 4, pszAppFlag);
		if(m_hMap && ::GetLastError() != ERROR_ALREADY_EXISTS)
			m_bIsFirstApp = TRUE;
		else if(!m_hMap)
		{
#ifdef _AFX
			ASSERT(FALSE);
#endif
		}
	}

	~COnlyOneApp() {if(m_hMap) {::CloseHandle(m_hMap); m_hMap = nullptr;}}

private:
	HANDLE	m_hMap;
	BOOL	m_bIsFirstApp;

	DECLARE_NO_COPY_CLASS(COnlyOneApp)
};

class COnlyOneWndApp
{
public:
	BOOL IsFirstApp()			{return m_hwndPre == nullptr;}
	HWND GetPreInstanceWindow()	{return m_hwndPre;}

	COnlyOneWndApp(LPCTSTR lpszClassName, LPCTSTR lpszWindowName = nullptr)
	{
		m_hwndPre = ::FindWindow(lpszClassName, lpszWindowName);
	}

private:
	HWND m_hwndPre;

	DECLARE_NO_COPY_CLASS(COnlyOneWndApp)
};

/************************************************************************/
/*                   	句柄 (HANDLE) 智能包装器 		 		        */
/************************************************************************/

template<HANDLE NULL_VALUE>
class auto_handle
{
public:
	auto_handle(HANDLE h = NULL_VALUE)			{set(h);}
	auto_handle(auto_handle<NULL_VALUE>& other)	{set(other.release());}

	~auto_handle()	{reset();}

	auto_handle<NULL_VALUE>& reset(HANDLE h = NULL_VALUE)
	{
		if(h != m_h)
		{
			if(is_valid())
				VERIFY(::CloseHandle(m_h));

			set(h);
		}

		return *this;
	}

	auto_handle<NULL_VALUE>& reset(auto_handle<NULL_VALUE>& other)
	{
		if(this != &other)
			reset(other.release());

		return *this;
	}

	HANDLE release()
	{
		HANDLE h = get();
		set(NULL_VALUE);
	
		return h;
	}

	operator HANDLE	()	const	{return m_h;}
	HANDLE get		()	const	{return m_h;}
	HANDLE& get_ref	()	const	{return m_h;}
	bool is_valid	()	const	{return m_h != NULL_VALUE;}

	auto_handle<NULL_VALUE>& operator = (HANDLE h)							{return reset(h);}
	auto_handle<NULL_VALUE>& operator = (auto_handle<NULL_VALUE>& other)	{return reset(other);}

	bool operator == (HANDLE h)								const	{return m_h == h;}
	bool operator != (HANDLE h)								const	{return !(operator == (h));	}
	bool operator == (const auto_handle<NULL_VALUE>& other)	const	{return m_h == other.m_h;}
	bool operator != (const auto_handle<NULL_VALUE>& other)	const	{return !(operator == (other));}

private:
	void set(HANDLE h)	{m_h = h;}

	// ~! do not define these conversion, because it's very easy to making mistake !~
	template<HANDLE _Other> auto_handle(const auto_handle<_Other>&);
	template<HANDLE _Other> auto_handle<NULL_VALUE>& operator = (const auto_handle<_Other>&);

private:
	HANDLE	m_h;
};

typedef auto_handle<INVALID_HANDLE_VALUE>	auto_file_handle;	// 文件智能句柄
typedef auto_handle<nullptr>				auto_res_handle;	// 普通资源智能句柄

/************************************************************************/
/*                 	  	 	DC 智能包装器 					            */
/************************************************************************/

class auto_dc
{
public:
	auto_dc(HDC h = nullptr, HWND w = nullptr, bool is_create = false)
	{
		set(h, w, is_create);
	}

	auto_dc(auto_dc& other)
	{
		set(other.m_h, other.m_w, other.m_is_create);
		other.release();
	}

	~auto_dc()	{reset();}

	HDC GetDC(HWND hWnd)
	{
		HDC h = ::GetDC(hWnd);
		reset(h, hWnd, false);

		return h;
	}

	HDC GetWindowDC(HWND hWnd)
	{
		HDC h = ::GetWindowDC(hWnd);
		reset(h, hWnd, false);

		return h;
	}

	HDC GetDCEx(HWND hWnd, HRGN hrgnClip, DWORD flags)
	{
		HDC h = ::GetDCEx(hWnd, hrgnClip, flags);
		reset(h, hWnd, false);

		return h;
	}

	HDC CreateDC(LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, CONST DEVMODE* lpInitData)
	{
		HDC h = ::CreateDC(lpszDriver, lpszDevice, lpszOutput, lpInitData);
		reset(h, nullptr, true);

		return h;
	}

	HDC CreateCompatibleDC(HDC hSrc)
	{
		HDC h = ::CreateCompatibleDC(hSrc);
		reset(h, nullptr, true);

		return h;
	}

	HGDIOBJ GetCurrentObject(UINT uObjectType)
	{
		return ::GetCurrentObject(m_h, uObjectType);
	}

	HBITMAP _GetCurrentBitmap()
	{
		return (HBITMAP)GetCurrentObject(OBJ_BITMAP);
	}

	HBRUSH _GetCurrentBrush()
	{
		return (HBRUSH)GetCurrentObject(OBJ_BRUSH);
	}

	HPALETTE _GetCurrentPalette()
	{
		return (HPALETTE)GetCurrentObject(OBJ_PAL);
	}

	HPEN _GetCurrentPen()
	{
		return (HPEN)GetCurrentObject(OBJ_PEN);
	}

	HFONT _GetCurrentFont()
	{
		return (HFONT)GetCurrentObject(OBJ_FONT);
	}

	int SelectClipRgn(HRGN hrgn)
	{
		return ::SelectClipRgn(m_h, hrgn);
	}

	int ExtSelectClipRgn(HRGN hrgn, int mode)
	{
		return ::ExtSelectClipRgn(m_h, hrgn, mode);
	}

	HGDIOBJ SelectObject(HGDIOBJ hgdiobj)
	{
		return ::SelectObject(m_h, hgdiobj);
	}

	HPALETTE SelectPalette(HPALETTE hpal, BOOL bForceBackground)
	{
		return ::SelectPalette(m_h, hpal, bForceBackground);
	}

	HBITMAP _SelectBitmap(HBITMAP hbm)
	{
		return (HBITMAP)SelectObject(hbm);
	}

	HBRUSH _SelectBrush(HBRUSH hbr)
	{
		return (HBRUSH)SelectObject(hbr);
	}

	HPEN _SelectPen(HPEN hpen)
	{
		return (HPEN)SelectObject(hpen);
	}

	HRGN _SelectRgn(HRGN hrgn)
	{
		return (HRGN)SelectObject(hrgn);
	}

	HFONT _SelectFont(HFONT hf)
	{
		return (HFONT)SelectObject(hf);
	}

	auto_dc& reset(HDC h = nullptr, HWND w = nullptr, bool is_create = false)
	{
		if(h != m_h || w != m_w)
		{
			if(is_valid())
			{
				if(m_is_create)
					VERIFY(::DeleteDC(m_h));
				else
					VERIFY(::ReleaseDC(m_w, m_h));
			}

			set(h, w, is_create);
		}

		return *this;
	}

	auto_dc& reset(auto_dc& other)
	{
		if(this != &other)
		{
			reset(other.m_h, other.m_w, other.m_is_create);
			other.release();
		}

		return *this;
	}

	HDC release()
	{
		HDC h = get_dc();
		set(nullptr, nullptr, false);

		return h;
	}

	/*
	auto_dc& operator = (HDC h)
	{
		return reset(h);
	}
	*/

	auto_dc& operator = (auto_dc& other)	{return reset(other);}

	operator	HDC			()	const	{return m_h;}
	HDC			get_dc		()	const	{return m_h;}
	const HDC&	get_dc_ref	()	const	{return m_h;}
	HWND		get_wnd		()	const	{return m_w;}
	bool		is_valid	()	const	{return m_h != nullptr;}
	bool		is_create	()	const	{return m_is_create;}

private:
	void set(HDC h, HWND w, bool is_create)
	{
		m_h			= h;
		m_w			= w;
		m_is_create	= is_create;
	}

private:
	HDC		m_h;
	HWND	m_w;
	bool	m_is_create;
};

class paint_dc
{
public:
	paint_dc(HWND hwnd) : m_hwnd(hwnd)
	{
		VERIFY(m_hdc = ::BeginPaint(m_hwnd, &m_ps));
	}

	~paint_dc()
	{
		VERIFY(::EndPaint(m_hwnd, &m_ps));
	}

	operator HDC ()	const	{return m_hdc;}
	bool is_valid()	const	{return m_hdc != nullptr;}

public:
	PAINTSTRUCT	m_ps;
	HWND		m_hwnd;
	HDC			m_hdc;
};
/************************************************************************/
/*                 	  	 	GDI Object 智能包装器 					    */
/************************************************************************/

template<class T>
class auto_gdi_obj
{
public:
	auto_gdi_obj(T obj = nullptr)				{set(obj);}
	auto_gdi_obj(auto_gdi_obj<T>& other)	{set(other.release());}

	~auto_gdi_obj()	{reset();}

	auto_gdi_obj<T>& reset(T obj = nullptr)
	{
		if(obj != m_obj)
		{
			if(is_valid())
			{
				VERIFY(::DeleteObject(m_obj));
			}

			set(obj);
		}

		return *this;
	}

	auto_gdi_obj<T>& reset(auto_gdi_obj<T>& other)
	{
		if(this != &other)
			reset(other.release());

		return *this;
	}

	T release()
	{
		T obj = get();
		set(nullptr);

		return obj;
	}

	auto_gdi_obj<T>& operator = (T obj)						{return reset(obj);}
	auto_gdi_obj<T>& operator = (auto_gdi_obj<T>& other)	{return reset(other);}

	operator	T		()	const	{return m_obj;}
	T			get		()	const	{return m_obj;}
	const T&	get_ref	()	const	{return m_obj;}
	bool		is_valid()	const	{return m_obj != nullptr;}

private:
	void set(T obj)	{m_obj = obj;}

protected:
	T m_obj;
};

typedef auto_gdi_obj<HBITMAP>		auto_bitmap_base;
typedef auto_gdi_obj<HBRUSH>		auto_brush_base;
typedef auto_gdi_obj<HPALETTE>		auto_palette_base;
typedef auto_gdi_obj<HPEN>			auto_pen_base;
typedef auto_gdi_obj<HRGN>			auto_rgn_base;
typedef auto_gdi_obj<HFONT>			auto_font_base;

class auto_bitmap : public auto_bitmap_base
{
public:
	auto_bitmap(HBITMAP obj = nullptr) : auto_bitmap_base(obj) {}

	auto_bitmap& operator = (HBITMAP obj)
	{
		return (auto_bitmap&)reset(obj);
	}

	HBITMAP CreateBitmap(int nWidth, int nHeight, UINT cPlanes, UINT cBitsPerPel, CONST VOID* lpvBits)
	{
		HBITMAP obj = ::CreateBitmap(nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits);
		reset(obj);

		return obj;
	}

#ifndef _WIN32_WCE
	HBITMAP CreateBitmapIndirect(CONST BITMAP *lpbm)
	{
		HBITMAP obj = ::CreateBitmapIndirect(lpbm);
		reset(obj);

		return obj;
	}
#else
	HBITMAP SHLoadDIBitMap(LPCTSTR lpszFileName)
	{
		HBITMAP obj = ::SHLoadDIBitmap(lpszFileName);
		reset(obj);

		return obj;
	}
#endif

	HBITMAP CreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight)
	{
		HBITMAP obj = ::CreateCompatibleBitmap(hdc, nWidth, nHeight);
		reset(obj);

		return obj;
	}

	HBITMAP CreateDIBSection(HDC hdc, const BITMAPINFO* pbmi, UINT iUsage, void** ppvBits, HANDLE hSection, DWORD dwOffset)
	{
		HBITMAP obj = ::CreateDIBSection(hdc, pbmi, iUsage, ppvBits, hSection, dwOffset);
		reset(obj);

		return obj;
	}

	HBITMAP LoadBitmap(HINSTANCE hInstance, LPCTSTR lpBitmapName)
	{
		HBITMAP obj = ::LoadBitmap(hInstance, lpBitmapName);
		reset(obj);

		return obj;
	}

	int GetBitmap(BITMAP* pBitMap)
	{
		ASSERT(m_obj != nullptr);
		return ::GetObject(m_obj, sizeof(BITMAP), pBitMap);
	}

	int GetDIBSection(DIBSECTION* pDIBSection)
	{
		ASSERT(m_obj != nullptr);
		return ::GetObject(m_obj, sizeof(DIBSECTION), pDIBSection);
	}
};

class auto_brush : public auto_brush_base
{
public:
	auto_brush(HBRUSH obj = nullptr) : auto_brush_base(obj) {}

	auto_brush& operator = (HBRUSH obj)
	{
		return (auto_brush&)reset(obj);
	}

	HBRUSH CreateDIBPatternBrushPt(const void* lpPackedDIB, UINT iUsage)
	{
		HBRUSH obj = ::CreateDIBPatternBrushPt(lpPackedDIB, iUsage);
		reset(obj);

		return obj;
	}

	HBRUSH CreatePatternBrush(HBITMAP hbmp)
	{
		HBRUSH obj = ::CreatePatternBrush(hbmp);
		reset(obj);

		return obj;
	}

	HBRUSH CreateSolidBrush(COLORREF crColor)
	{
		HBRUSH obj = ::CreateSolidBrush(crColor);
		reset(obj);

		return obj;
	}

	int GetLogBrush(LOGBRUSH* pLogBrush)
	{
		ASSERT(m_obj != nullptr);
		return ::GetObject(m_obj, sizeof(LOGBRUSH), pLogBrush);
	}
};

class auto_palette : public auto_palette_base
{
public:
	auto_palette(HPALETTE obj = nullptr) : auto_palette_base(obj) {}

	auto_palette& operator = (HPALETTE obj)
	{
		return (auto_palette&)reset(obj);
	}

	HPALETTE CreatePalette(CONST LOGPALETTE* lplgpl)
	{
		HPALETTE obj = ::CreatePalette(lplgpl);
		reset(obj);

		return obj;
	}

	int GetEntryCount()
	{
		ASSERT(m_obj != nullptr);

		WORD nEntries;
		::GetObject(m_obj, sizeof(WORD), &nEntries);
		return (int)nEntries;
	}
};

class auto_pen : public auto_pen_base
{
public:
	auto_pen(HPEN obj = nullptr) : auto_pen_base(obj) {}

	auto_pen& operator = (HPEN obj)
	{
		return (auto_pen&)reset(obj);
	}

	HPEN CreatePen(int fnPenStyle, int nWidth, COLORREF crColor)
	{
		HPEN obj = ::CreatePen(fnPenStyle, nWidth, crColor);
		reset(obj);

		return obj;
	}

	HPEN CreatePenIndirect(const LOGPEN* lplgpn)
	{
		HPEN obj = ::CreatePenIndirect(lplgpn);
		reset(obj);

		return obj;
	}

	int GetLogPen(LOGPEN* pLogPen)
	{
		ASSERT(m_obj != nullptr);
		return ::GetObject(m_obj, sizeof(LOGPEN), pLogPen);
	}
};

class auto_rgn : public auto_rgn_base
{
public:
	auto_rgn(HRGN obj = nullptr) : auto_rgn_base(obj) {}

	auto_rgn& operator = (HRGN obj)
	{
		return (auto_rgn&)reset(obj);
	}

	HRGN CreateRectRgn(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect)
	{
		HRGN obj = ::CreateRectRgn(nLeftRect, nTopRect, nRightRect, nBottomRect);
		reset(obj);

		return obj;
	}

	HRGN CreateRectRgnIndirect(const RECT* lprc)
	{
		HRGN obj = ::CreateRectRgnIndirect(lprc);
		reset(obj);

		return obj;
	}
};

class auto_font : public auto_font_base
{
public:
	auto_font(HFONT obj = nullptr) : auto_font_base(obj) {}

	auto_font& operator = (HFONT obj)
	{
		return (auto_font&)reset(obj);
	}

	HFONT CreateFont(
						int nHeight,				// height of font
						int nWidth,					// average character width
						int nEscapement,			// angle of escapement
						int nOrientation,			// base-line orientation angle
						int fnWeight,				// font weight
#ifndef _WIN32_WCE
						DWORD bItalic,				// italic attribute option
						DWORD bUnderline,			// underline attribute option
						DWORD cStrikeOut,			// strikeout attribute option
						DWORD nCharSet,				// character set identifier
						DWORD nOutPrecision,		// output precision
						DWORD nClipPrecision,		// clipping precision
						DWORD nQuality,				// output quality
						DWORD nPitchAndFamily,		// pitch and family
#else
						BYTE bItalic,				// italic attribute option
						BYTE bUnderline,			// underline attribute option
						BYTE cStrikeOut,			// strikeout attribute option
						BYTE nCharSet,				// character set identifier
						BYTE nOutPrecision,			// output precision
						BYTE nClipPrecision,		// clipping precision
						BYTE nQuality,				// output quality
						BYTE nPitchAndFamily,		// pitch and family

#endif
						LPCTSTR lpszFace           // typeface name
					)
	{
		HFONT obj = ::CreateFont(
									nHeight,
									nWidth,
									nEscapement,
									nOrientation,
									fnWeight,
									bItalic,
									bUnderline,
									cStrikeOut,
									nCharSet,
									nOutPrecision,
									nClipPrecision,
									nQuality,
									nPitchAndFamily,
									lpszFace
								);
		reset(obj);

		return obj;
	}

	HFONT CreateFontIndirect(const LOGFONT* lplf)
	{
		HFONT obj = ::CreateFontIndirect(lplf);
		reset(obj);

		return obj;
	}

	int GetLogFont(LOGFONT* pLogFont)
	{
		ASSERT(m_obj != nullptr);
		return ::GetObject(m_obj, sizeof(LOGFONT), pLogFont);
	}
};
