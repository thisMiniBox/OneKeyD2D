#pragma once
#include<d2d1.h>
/// <summary>
/// �ͷ�d2d�Ⱥ���Release�����ľ��
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
/// �ͷ�new������
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
/// �ͷ�win32gdi��ʹ��DeleteObject�����ͷ���Դ�ľ��
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
        // ʵ����ʽ
        SolidLine = PS_SOLID,

        // ������ʽ
        DashedLine = PS_DASH,

        // ������ʽ
        DotLine = PS_DOT,

        // ���ߵ�����ʽ
        DashDot = PS_DASHDOT,

        // ˫������ʽ
        DashDotDot = PS_DASHDOTDOT,

        // ����ʽ������������
        NullLine = PS_NULL,

        // �ڿ���ʽ
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
                std::cerr << "��ʼ��ʧ��" << std::endl;
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