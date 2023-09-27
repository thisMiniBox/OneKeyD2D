#pragma once
#include<d2d1.h>
/// <summary>
/// 释放d2d等含有Release函数的句柄
/// </summary>
/// <typeparam name="Interface"></typeparam>
/// <param name="ppInterfaceToRelease"></param>
template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}
/// <summary>
/// 释放new的内容
/// </summary>
/// <typeparam name="Interface"></typeparam>
/// <param name="ppInterfaceToRelease"></param>
template<class Interface>
inline void SafeReleaseP(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        delete (*ppInterfaceToRelease);
        (*ppInterfaceToRelease) = NULL;
    }
}
/// <summary>
/// 释放win32gdi等使用DeleteObject函数释放资源的句柄
/// </summary>
/// <typeparam name="Interface"></typeparam>
/// <param name="ppInterfaceToRelease"></param>
template<class Interface>
inline void SafeReleaseW(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        DeleteObject(*ppInterfaceToRelease);
        (*ppInterfaceToRelease) = NULL;
    }
}

class GetLastWindowError
{
    DWORD m_ErrorCode;
    LPSTR m_ErrorMessage;
public:
    GetLastWindowError()
    {
        m_ErrorCode = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, m_ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&m_ErrorMessage, 0, NULL);
    }
    GetLastWindowError(HRESULT hr)
    {
        m_ErrorCode = HRESULT_CODE(hr);
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, m_ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&m_ErrorMessage, 0, NULL);
    }
    ~GetLastWindowError()
    {
        if(m_ErrorMessage)
            LocalFree(m_ErrorMessage);
    }
    DWORD GetErrorCode() const
    {
        return m_ErrorCode;
    }
    LPSTR GetErrorMessage() const
    {
        return m_ErrorMessage;
    }
};

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#include<string>

#ifdef UNICODE
    
typedef std::wstring STR;
typedef const std::wstring CSTR;
typedef std::wstring& QSTR;
typedef const std::wstring& CQSTR;

#else

typedef std::string STR;
typedef const std::string CSTR;
typedef std::string& QSTR;
typedef const std::string& CQSTR;

#endif // UNICODE

namespace Game {
    enum KeyMode
    {
        KM_CHAR,
        KM_DOWN,
        KM_UP,
    };
    enum MouseMessageType
    {
        MT_MOVE,
        MT_LEFT,
        MT_DOUBLE_LEFT,
        MT_RIGHT,
        MT_DOUBLE_RIGHT,
        MT_MID,
        MT_DOUBLE_MID,
        MT_WHEEL,
        MT_ENTER_WINDOW,
        MT_LEAVE_WINDOW,
    };

    enum class PenStyle
    {
        // 实线样式
        SolidLine = PS_SOLID,

        // 虚线样式
        DashedLine = PS_DASH,

        // 点线样式
        DotLine = PS_DOT,

        // 虚线点线样式
        DashDot = PS_DASHDOT,

        // 双点线样式
        DashDotDot = PS_DASHDOTDOT,

        // 空样式，不绘制轮廓
        NullLine = PS_NULL,

        // 内框样式
        InsideFrame = PS_INSIDEFRAME,
    };
    class AutoInit
    {
    public:
        AutoInit()
        {
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr))
            {
                std::cerr << "初始化失败" << std::endl;
                return;
            }
            return;
        }
        ~AutoInit()
        {
            CoUninitialize();
        }
    };
}