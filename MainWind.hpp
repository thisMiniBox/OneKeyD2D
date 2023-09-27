#pragma once

#include<d2d1.h>
#include<windows.h>
#include<string>
#include<iostream>
#include<vector>
#include<set>
#include<dwrite.h>
#include"D2Dmacro.hpp"

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Dwrite.lib")


#define MAINWIND_HPP
namespace Game 
{
	class MainWind;
	class MainWind_GDI;
	class MainWind_D2D;



	namespace WindClassName {
		constexpr auto MAIN_WINDOW_GDI_CLASS_NAME = TEXT("MainWindow_Game");
		constexpr auto MAIN_WINDOW_D2D_CLASS_NAME = TEXT("d2dMainWindow_Game");
	}
	namespace WindCount
	{
		static int g_gdiWindCount = 0;
		static int g_d2dMainWindCount = 0;
	}
	namespace WindCallback
	{
		typedef void(*CharInputCallback_GDI)(MainWind_GDI*, int KeyInputChar, int Frequency, KeyMode);
		typedef void(*PaintCallback_GDI)(MainWind_GDI*);
		typedef void(*SizeCallback_GDI)(MainWind_GDI*, int width, int height);
		typedef void(*CharInputCallback_D2D)(MainWind_D2D*, int KeyInputChar, int Frequency, KeyMode);
		typedef void(*PaintCallback_D2D)(MainWind_D2D*);
		typedef void(*SizeCallback_D2D)(MainWind_D2D*, int width, int height);

		typedef void(*ButtonCallback)(MainWind*);
		typedef void(*MouseCallback)(MainWind*, int x, int y, int VirtualKey, MouseMessageType, KeyMode);
	}




	namespace WindControl
	{
		class d2dClickDetection
		{
		private:
			MainWind* m_TargetWind;
			float m_X;
			float m_Y;
			float m_RightX;
			float m_RightY;
			WindCallback::ButtonCallback m_Callback;
		public:
			d2dClickDetection(float x = 0, float y = 0, float w = 0, float h = 0) :m_RightX(x+w), m_RightY(h+y)
			{
				m_X = x;
				m_Y = y;
			}
			void SetRectangle(const D2D_RECT_F& checkRect)
			{
				m_X = checkRect.left;
				m_Y = checkRect.top;
				m_RightX = checkRect.right;
				m_RightY = checkRect.bottom;
			}
			D2D_RECT_F GetRectangle()const
			{
				return { m_X,m_Y,m_RightX,m_RightY };
			}
			void SetPosition(float x, float y)
			{
				float wDifference = x - m_X;
				float hDifference = y - m_Y;
				m_X += wDifference;
				m_Y += hDifference;
				m_RightX += wDifference;
				m_RightY += hDifference;
			}
			void SetWide(float width, float height)
			{

				m_RightY = height + m_Y;
				m_RightX = width + m_X;
			}
			bool CheckClick(float x, float y)const
			{
				if (x<m_X || y<m_Y || x>m_RightX || y> m_RightY)
					return false;
				return true;
			}
			void SetButtonCallback(WindCallback::ButtonCallback bc)
			{
				m_Callback = bc;
			}
			friend class MainWind;
		};
	}



	///////////////////以下是窗口类///////////////////



	class MainWind
	{
	protected:
		HWND m_hWnd;
		HINSTANCE m_hInstance;
		LPARAM m_UserData;

		WindCallback::MouseCallback m_MouseCallback;
		std::set<WindControl::d2dClickDetection*>m_Buttons;
	
		virtual void OnPaint() = 0;
		virtual void OnSize(int width,int height) = 0;
		void OnMouse(UINT msg, int x, int y, int VirtualKey)
		{
			if (msg == WM_LBUTTONDOWN)
			{
				for (auto& bt : m_Buttons)
				{
					if (!bt->CheckClick(x, y))
						continue;
					bt->m_Callback(this);
					break;
				}
			}
			if (!m_MouseCallback)
				return;
			switch (msg)
			{
			case WM_MOUSEMOVE:
				m_MouseCallback(this, x, y, VirtualKey, MT_MOVE, KM_CHAR);
				break;
			case WM_LBUTTONDBLCLK:
				m_MouseCallback(this, x, y, VirtualKey, MT_DOUBLE_LEFT, KM_DOWN);
				break;
			case WM_LBUTTONDOWN:
				m_MouseCallback(this, x, y, VirtualKey, MT_LEFT, KM_DOWN);
				break;
			case WM_LBUTTONUP:
				m_MouseCallback(this, x, y, VirtualKey, MT_LEFT, KM_UP);
				break;
			case WM_RBUTTONDBLCLK:
				m_MouseCallback(this, x, y, VirtualKey, MT_DOUBLE_RIGHT, KM_DOWN);
				break;
			case WM_RBUTTONDOWN:
				m_MouseCallback(this, x, y, VirtualKey, MT_RIGHT, KM_DOWN);
				break;
			case WM_RBUTTONUP:
				m_MouseCallback(this, x, y, VirtualKey, MT_RIGHT, KM_UP);
				break;
			case WM_MBUTTONDBLCLK:
				m_MouseCallback(this, x, y, VirtualKey, MT_DOUBLE_MID, KM_DOWN);
				break;
			case WM_MBUTTONDOWN:
				m_MouseCallback(this, x, y, VirtualKey, MT_MID, KM_DOWN);
				break;
			case WM_MBUTTONUP:
				m_MouseCallback(this, x, y, VirtualKey, MT_MID, KM_UP);
				break;
			case WM_MOUSEWHEEL:
				if (GET_WHEEL_DELTA_WPARAM(VirtualKey) > 0)
					m_MouseCallback(this, x, y, VirtualKey, MT_WHEEL, KM_UP);
				else
					m_MouseCallback(this, x, y, VirtualKey, MT_WHEEL, KM_DOWN);
				break;
			case WM_MOUSEACTIVATE:
				m_MouseCallback(this, x, y, VirtualKey, MT_ENTER_WINDOW, KM_CHAR);
				break;
			case WM_MOUSELEAVE:
				m_MouseCallback(this, x, y, VirtualKey, MT_LEAVE_WINDOW, KM_CHAR);
				break;
			default:
				break;
			}
		}

		static LONG_PTR GetWindLongPtr(HWND hWnd)
		{
			return (
				::GetWindowLongPtrW(
					hWnd,
					GWLP_USERDATA
				));
		}
	public:

		MainWind() : m_hWnd(nullptr), m_hInstance(nullptr), m_UserData(0) {}
		virtual ~MainWind(){}
		virtual HRESULT CreatWind(HWND parent = nullptr, HINSTANCE hInstance = HINST_THISCOMPONENT) = 0;

		void SetMouseCallback(WindCallback::MouseCallback mcb)
		{
			m_MouseCallback = mcb;
		}

		/// <summary>
		/// 设置默认画笔,当宽度大于1时，样式无效
		/// </summary>
		/// <param name="color"></param>
		/// <param name="penStyle"></param>
		/// <param name="LineWidth"></param>
		/// <returns></returns>
		virtual bool SetPen(COLORREF color, PenStyle penStyle = PenStyle::SolidLine, int LineWidth = 1) = 0;
		virtual bool ClearWindBackground(COLORREF color = RGB(255, 255, 255)) = 0;
		virtual bool AutoClearWindBackground(COLORREF color = RGB(255, 255, 255), bool run = true) = 0;
		/// <summary>
		/// 绘制矩形，颜色由画笔决定,仅在绘制过程函数调用
		/// </summary>
		/// <param name="left">左上角x</param>
		/// <param name="top">左上角y</param>
		/// <param name="right">右下角x</param>
		/// <param name="bottom">右下角y</param>
		/// <returns></returns>
		virtual bool DrawRectangle(
			int left,
			int top,
			int right,
			int bottom) = 0;
		/// <summary>
		/// 画线,颜色为画笔颜色
		/// </summary>
		/// <param name="startX"></param>
		/// <param name="startY"></param>
		/// <param name="endX"></param>
		/// <param name="endY"></param>
		/// <returns></returns>
		virtual bool DrawLine(int startX, int startY, int endX, int endY) = 0;
		/// <summary>
		/// 画线,颜色为画笔颜色
		/// 从上一次结束的地方开始，默认为（0，0）开始
		/// </summary>
		/// <param name="toX"></param>
		/// <param name="toY"></param>
		/// <returns></returns>
		virtual bool DrawLineTo(int toX, int toY) = 0;
		
		virtual bool DrawTextAscii(const std::string& ShowText, int x, int y, int w, int h) = 0;
		virtual bool DrawTextWide(CQSTR ShowText, int x, int y, int w, int h) = 0;

		/// <summary>
		/// 获取窗口句柄（如果对win32编程不熟悉不建议直接使用句柄）
		/// </summary>
		/// <returns></returns>
		HWND GethWnd()const
		{
			return m_hWnd;
		}
		void CloseWind()
		{
			if (m_hWnd)
				DestroyWindow(m_hWnd);
			m_hWnd = nullptr;
		}
		//启动消息循环，直到窗口关闭
		static void StaticWindowMessageLoop()
		{
			MSG msg = { };
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		/// <summary>
		/// 强制重绘窗口
		/// </summary>
		void PaintWInd()
		{
			if (m_hWnd)
				InvalidateRect(m_hWnd, NULL, false);
		}

		SIZE GetWindSize()const
		{
			RECT rect;
			GetClientRect(m_hWnd, &rect);
			return { rect.right,rect.bottom };
		}
		RECT GetWindRect()const
		{
			RECT rect;
			GetWindowRect(m_hWnd, &rect);
			return rect;
		}
		void MoveWind(int x, int y, int w, int h)const
		{
			MoveWindow(m_hWnd, x, y, w, h, true);
		}
		void SetText(CQSTR text)const
		{
			SetWindowText(m_hWnd, text.c_str());
		}
		void ShowWind()const
		{
			ShowWindow(m_hWnd, SW_SHOW);
		}
		void HideWind()const
		{
			ShowWindow(m_hWnd, SW_HIDE);
		}
		void AddButten(WindControl::d2dClickDetection* db)
		{
			m_Buttons.insert(db);
		}
		void DeleteButten(WindControl::d2dClickDetection* db)
		{
			m_Buttons.erase(db);
		}
		void SetUserData(LPARAM data)
		{
			m_UserData = data;
		}
		LPARAM GetUserData()const
		{
			return m_UserData;
		}

	};

	class MainWind_GDI :public MainWind
	{
	private:
		HBRUSH m_BackgroundColor;
		HDC m_hdc;
		RECT m_WindWide;

		HPEN m_CurrentPen;
		HPEN m_OldPen;
		WindCallback::CharInputCallback_GDI m_CharCallback;
		WindCallback::PaintCallback_GDI m_PaintCallback;
		WindCallback::SizeCallback_GDI m_WindSizeCallback;

		void OnPaint()override
		{
			PAINTSTRUCT ps;
			m_hdc = BeginPaint(m_hWnd, &ps);
			if (m_CurrentPen)
				m_OldPen = (HPEN)SelectObject(m_hdc, m_CurrentPen);
			if (m_BackgroundColor)
				FillRect(m_hdc, &m_WindWide, m_BackgroundColor);
			if (m_PaintCallback)
				m_PaintCallback(this);
			if (m_CurrentPen)
				SelectObject(m_hdc, m_OldPen);
			EndPaint(m_hWnd, &ps);
			m_hdc = nullptr;
		}
		void OnSize(int width, int height)override
		{
			m_WindWide.right = width;
			m_WindWide.bottom = height;
			if (m_WindSizeCallback)
				m_WindSizeCallback(this, width, height);
		}
		static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (uMsg == WM_CREATE)
			{
				LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
				MainWind_GDI* mainWind = (MainWind_GDI*)pcs->lpCreateParams;

				::SetWindowLongPtrW(
					hWnd,
					GWLP_USERDATA,
					reinterpret_cast<LONG_PTR>(mainWind)
				);
				return 1;
			}
			MainWind_GDI* mainWind = reinterpret_cast<MainWind_GDI*>(GetWindLongPtr(hWnd));
			if (!mainWind)
			{
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			if (uMsg > WM_MOUSEFIRST && uMsg < WM_MOUSELAST||uMsg==WM_MOUSEACTIVATE||uMsg== WM_MOUSELEAVE)
			{
				mainWind->OnMouse(uMsg, LOWORD(lParam), HIWORD(lParam), wParam);
				return 0;
			}
			switch (uMsg)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			{
				if (mainWind->m_CharCallback)
				{
					switch (uMsg)
					{
					case WM_CHAR:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_CHAR);
						break;
					case WM_KEYDOWN:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_DOWN);
						break;
					case WM_KEYUP:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_UP);
						break;
					default:
						break;
					}
				}
				break;
			}
			case WM_PAINT:
			{
				mainWind->OnPaint();
				break;
			}
			case WM_SIZE:
			{
				mainWind->OnSize(LOWORD(lParam), HIWORD(lParam));
				break;
			}
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
				break;
			}
			return 0;
		}
	public:
		MainWind_GDI() :m_hdc(nullptr), m_BackgroundColor(nullptr), m_CurrentPen(nullptr), m_OldPen(nullptr),
			m_CharCallback(nullptr), m_PaintCallback(nullptr), m_WindSizeCallback(nullptr)
		{ 
			WindCount::g_gdiWindCount++;
			m_WindWide = { 0,0,0,0 };
		}
		~MainWind_GDI()
		{
			SafeReleaseW(&m_CurrentPen);
			if (m_BackgroundColor)
				DeleteObject(m_BackgroundColor);
			WindCount::g_gdiWindCount--;
			CloseWind();
			if (WindCount::g_gdiWindCount == 0)
			{
				UnregisterClass(WindClassName::MAIN_WINDOW_GDI_CLASS_NAME, m_hInstance);
			}
		}
		HRESULT CreatWind(HWND parent = nullptr, HINSTANCE hInstance = HINST_THISCOMPONENT)override
		{
			if (m_hWnd)
			{
				std::cerr << "请使用新的实例创建窗口" << std::endl;
				return S_FALSE;
			}
			m_hInstance = hInstance;

			WNDCLASS windowClass = {};
			if (GetClassInfo(hInstance, WindClassName::MAIN_WINDOW_GDI_CLASS_NAME, &windowClass) == 0)
			{
				WNDCLASSEX wc = { 0 };
				wc.cbSize = sizeof(WNDCLASSEX);
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.lpfnWndProc = MainWindowProc;
				wc.hInstance = hInstance;
				wc.hCursor = LoadCursor(NULL, IDC_ARROW);
				wc.lpszClassName = WindClassName::MAIN_WINDOW_GDI_CLASS_NAME;

				// 注册窗口类
				if (!RegisterClassEx(&wc)) {
					GetLastWindowError error;
					std::string errorOutput = "窗口类注册失败!";
					errorOutput += error.GetErrorMessage();
					MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
					return S_FALSE;
				}
			}

			if (parent)
			{
				RECT parentRect;
				GetClientRect(parent, &parentRect);
				m_hWnd = CreateWindowEx(
					NULL, WindClassName::MAIN_WINDOW_GDI_CLASS_NAME,
					TEXT("gdi窗口"),
					WS_CHILD | WS_VISIBLE,
					0, 0, parentRect.right, parentRect.bottom,
					parent, NULL,
					hInstance, this);
			}
			else
			{
				m_hWnd = CreateWindowEx(
					NULL, WindClassName::MAIN_WINDOW_GDI_CLASS_NAME,
					TEXT("gdi窗口"),
					WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					100, 100, 800, 600,
					NULL, NULL,
					hInstance, this);
			}
			if (!m_hWnd)
			{
				GetLastWindowError error;
				std::string errorOutput = "窗口创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return S_FALSE;
			}
			GetClientRect(m_hWnd, &m_WindWide);
			return S_OK;
		}


		/// <summary>
		/// 设置字符消息的处理函数
		/// </summary>
		/// <param name="charCallback"></param>
		void SetKeyCallback(WindCallback::CharInputCallback_GDI charCallback)
		{
			m_CharCallback = charCallback;
		}
		/// <summary>
		/// 设置窗口绘制过程函数
		/// </summary>
		/// <param name="paintCallback"></param>
		void SetPaintCallback(WindCallback::PaintCallback_GDI paintCallback)
		{
			m_PaintCallback = paintCallback;
		}
		/// <summary>
		/// 窗口大小改变时调用
		/// </summary>
		/// <param name="sizeCallback"></param>
		void SetWindSizeCallback(WindCallback::SizeCallback_GDI sizeCallback)
		{
			m_WindSizeCallback = sizeCallback;
		}


		bool DrawRectangle(
			int left,
			int top,
			int right,
			int bottom)override
		{
			if (m_hdc)
				return Rectangle(m_hdc, left, top, right, bottom);
			return false;
		}
		bool ClearWindBackground(COLORREF color = RGB(255, 255, 255))override
		{
			if (!m_hdc)
				return false;
			RECT clientRect;
			GetClientRect(m_hWnd, &clientRect);
			HBRUSH hBrush = CreateSolidBrush(color);
			FillRect(m_hdc, &clientRect, hBrush);
			DeleteObject(hBrush);
			return true;
		}
		bool AutoClearWindBackground(COLORREF color = RGB(255, 255, 255), bool run = true)
		{
			if (!run)
			{
				if (m_BackgroundColor)
				{
					DeleteObject(m_BackgroundColor);
				}
				m_BackgroundColor = nullptr;
				return true;
			}
			if (m_BackgroundColor)
			{
				DeleteObject(m_BackgroundColor);
			}
			m_BackgroundColor = CreateSolidBrush(color);
			if (m_BackgroundColor)
				return false;
			return true;
		}
		bool DrawLine(int startX, int startY, int endX, int endY)
		{
			if (!m_hdc)
				return false;
			MoveToEx(m_hdc, startX, startY, nullptr);
			LineTo(m_hdc, endX, endY);
			return true;
		}
		bool DrawLineTo(int toX, int toY)
		{
			if (!m_hdc)
				return false;
			LineTo(m_hdc,toX, toY);
			return true;
		}
		bool DrawTextAscii(const std::string& ShowText, int x, int y, int w, int h)override
		{
			if (!m_hdc)
				return false;
			RECT rect = { x,y,x + w,y + h };
			DrawTextA(m_hdc, ShowText.c_str(), ShowText.size(), &rect, DT_CENTER);
		}
		bool DrawTextWide(CQSTR ShowText, int x, int y, int w, int h)override
		{
			if (!m_hdc)
				return false;
			RECT rect = { x,y,x + w,y + h };
			DrawTextW(m_hdc, ShowText.c_str(), ShowText.size(), &rect, DT_CENTER);
		}


		bool SetPen(COLORREF color, PenStyle penStyle = PenStyle::SolidLine, int LineWidth = 1)override
		{
			if (m_hdc)
			{
				auto lastPen = m_CurrentPen;
				m_CurrentPen = CreatePen((int)penStyle, LineWidth, color);
				if (!m_CurrentPen)
				{
					m_CurrentPen = lastPen;
					return false;
				}
				SelectObject(m_hdc, m_CurrentPen);
				SafeReleaseW(&lastPen);
				return true;
			}
			SafeReleaseW(&m_CurrentPen);
			m_CurrentPen = CreatePen((int)penStyle, LineWidth, color);
			if (m_CurrentPen)
				return true;
			return false;
		}
	};
	
	class MainWind_D2D :public MainWind
	{
	private:
		ID2D1Factory* m_d2dFactory;
		ID2D1HwndRenderTarget* m_d2dRenderTarget;

		D2D1_POINT_2F m_LastPoint;
		int m_PenWidth;
		ID2D1SolidColorBrush* m_PenBrush;
		ID2D1StrokeStyle* m_PenStyle;

		IDWriteTextFormat* m_TextFormat;

		COLORREF* m_BackgroundColor;


		FILETIME m_LastPaintTime;
		FILETIME m_NowTime;
		float m_IntervalTime;

		WindCallback::CharInputCallback_D2D m_CharCallback;
		WindCallback::PaintCallback_D2D m_PaintCallback;
		WindCallback::SizeCallback_D2D m_WindSizeCallback;
		void OnPaint()override
		{
			if (!m_d2dFactory || !m_d2dRenderTarget)
			{
				PAINTSTRUCT ps;
				BeginPaint(m_hWnd, &ps);
				EndPaint(m_hWnd, &ps);
				return;
			}
			GetSystemTimePreciseAsFileTime(&m_NowTime);
			m_IntervalTime = ((*reinterpret_cast<const ULONGLONG*>(&m_NowTime)) - *reinterpret_cast<const ULONGLONG*>(&m_LastPaintTime)) / 10000.f;
			m_d2dRenderTarget->BeginDraw();
			if (m_BackgroundColor)
				m_d2dRenderTarget->Clear(D2D1::ColorF(*m_BackgroundColor));
			if (m_PaintCallback)
				m_PaintCallback(this);
			m_d2dRenderTarget->EndDraw();
			m_LastPaintTime = m_NowTime;
			return;
		}
		void OnSize(int width, int height)override
		{
			if (!m_d2dFactory)
				return;
			HRESULT hr = m_d2dRenderTarget->Resize(D2D1::SizeU(width, height));
			if (FAILED(hr))
			{
				GetLastWindowError error;
				std::wcerr << L"d2d窗口输出对象创建失败！在窗口大小调整过程中\n" <<error.GetErrorMessage()<< std::endl;
				return;
			}
			if (m_WindSizeCallback)
				m_WindSizeCallback(this, width, height);
		}
		static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (uMsg == WM_CREATE)
			{
				LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
				MainWind_D2D* mainWind = (MainWind_D2D*)pcs->lpCreateParams;

				::SetWindowLongPtrW(
					hWnd,
					GWLP_USERDATA,
					reinterpret_cast<LONG_PTR>(mainWind)
				);
				return 1;
			}
			MainWind_D2D* mainWind = reinterpret_cast<MainWind_D2D*>(GetWindLongPtr(hWnd));
			if (!mainWind)
			{
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			if (uMsg > WM_MOUSEFIRST && uMsg < WM_MOUSELAST || uMsg == WM_MOUSEACTIVATE || uMsg == WM_MOUSELEAVE)
			{
				mainWind->OnMouse(uMsg, LOWORD(lParam), HIWORD(lParam), wParam);
				return 0;
			}
			switch (uMsg)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			{
				if (mainWind->m_CharCallback)
				{
					switch (uMsg)
					{
					case WM_CHAR:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_CHAR);
						break;
					case WM_KEYDOWN:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_DOWN);
						break;
					case WM_KEYUP:
						mainWind->m_CharCallback(mainWind, (int)wParam, LOWORD(lParam), KM_UP);
						break;
					default:
						break;
					}
				}
				break;
			}
			case WM_PAINT:
			{
				mainWind->OnPaint();
				break;
			}
			case WM_SIZE:
			{
				mainWind->OnSize(LOWORD(lParam), HIWORD(lParam));
				break;
			}

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
				break;
			}
			return 0;
		}
	public:
		MainWind_D2D() :m_d2dFactory(nullptr), m_d2dRenderTarget(nullptr), m_PenBrush(nullptr), m_LastPoint({ 0,0 }),
			m_PenStyle(nullptr), m_PenWidth(1), m_CharCallback(nullptr), m_PaintCallback(nullptr), m_WindSizeCallback(nullptr),
			m_TextFormat(nullptr), m_LastPaintTime(),m_NowTime()
		{
			WindCount::g_d2dMainWindCount++;
		}
		~MainWind_D2D()
		{
			WindCount::g_d2dMainWindCount--;
			SafeRelease(&m_d2dFactory);
			SafeRelease(&m_d2dRenderTarget);
			SafeRelease(&m_PenBrush);
			SafeRelease(&m_PenStyle);
			SafeReleaseP(&m_BackgroundColor);
			SafeRelease(&m_TextFormat);
			CloseWind();
			if (WindCount::g_d2dMainWindCount == 0)
			{
				UnregisterClass(WindClassName::MAIN_WINDOW_D2D_CLASS_NAME, m_hInstance);
			}
		}
		ID2D1HwndRenderTarget* GetD2DTargetP()const
		{
			return m_d2dRenderTarget;
		}
		ID2D1Factory* GetD2DFactoryP()const
		{
			return m_d2dFactory;
		}
		HRESULT CreatWind(HWND parent = nullptr, HINSTANCE hInstance = HINST_THISCOMPONENT)override
		{
			if (m_hWnd)
			{ 
				std::cerr << "请使用新的实例创建窗口" << std::endl;
				return S_FALSE;
			}
			m_hInstance = hInstance;

			WNDCLASS windowClass = {};
			if (GetClassInfo(hInstance, WindClassName::MAIN_WINDOW_D2D_CLASS_NAME, &windowClass) == 0)
			{
				WNDCLASSEX wc = { 0 };
				wc.cbSize = sizeof(WNDCLASSEX);
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.lpfnWndProc = MainWindowProc;
				wc.hInstance = hInstance;
				wc.hCursor = LoadCursor(NULL, IDC_ARROW);
				wc.lpszClassName = WindClassName::MAIN_WINDOW_D2D_CLASS_NAME;

				// 注册窗口类
				if (!RegisterClassEx(&wc)) {
					GetLastWindowError error;
					std::string errorOutput = "窗口类注册失败!";
					errorOutput += error.GetErrorMessage();
					MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
					return S_FALSE;
				}
			}

			if (parent)
			{
				RECT parentRect;
				GetClientRect(parent, &parentRect);
				m_hWnd = CreateWindowEx(
					NULL, WindClassName::MAIN_WINDOW_D2D_CLASS_NAME,
					TEXT("d2d窗口"),
					WS_CHILD|WS_VISIBLE,
					0, 0, parentRect.right, parentRect.bottom,
					parent, NULL,
					hInstance, this);
			}
			else
			{
				m_hWnd = CreateWindowEx(
					NULL, WindClassName::MAIN_WINDOW_D2D_CLASS_NAME,
					TEXT("d2d窗口"),
					WS_OVERLAPPEDWINDOW|WS_VISIBLE,
					100, 100, 800, 600,
					NULL, NULL,
					hInstance, this);
			}
			if (!m_hWnd)
			{
				GetLastWindowError error;
				std::string errorOutput = "窗口创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return S_FALSE;
			}
			HRESULT hr = D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED,
				&m_d2dFactory
			);
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d设备创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			hr = m_d2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(
					m_hWnd,
					D2D1::SizeU(
						rc.right - rc.left,
						rc.bottom - rc.top)
				), &m_d2dRenderTarget);
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d设备关联窗口失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}
			hr = m_d2dRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_PenBrush);
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d画笔创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}

			IDWriteFactory* writeFactory = nullptr;
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d文本接口创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}
			hr = writeFactory->CreateTextFormat(
				L"Gabriola",
				NULL,
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				30.0f,
				L"en-us",
				&m_TextFormat
			);
			SafeRelease(&writeFactory);
			if (FAILED(hr))
			{
				GetLastWindowError error(hr);
				std::string errorOutput = "d2d字体创建失败!";
				errorOutput += error.GetErrorMessage();
				MessageBoxA(NULL, errorOutput.c_str(), "错误", MB_ICONERROR);
				return hr;
			}
			GetSystemTimePreciseAsFileTime(&m_LastPaintTime);
			return S_OK;
		}

		COLORREF D2DColorFToGdiColor(D2D1::ColorF colorF)
		{
			BYTE redByte = static_cast<BYTE>(colorF.r * 255);
			BYTE greenByte = static_cast<BYTE>(colorF.g * 255);
			BYTE blueByte = static_cast<BYTE>(colorF.b * 255);
			return RGB(redByte, greenByte, blueByte);
		}
		D2D1::ColorF GdiColorToD2DColor(COLORREF colorRef)
		{
			float red = GetRValue(colorRef) / 255.0f;
			float green = GetGValue(colorRef) / 255.0f;
			float blue = GetBValue(colorRef) / 255.0f;
			return D2D1::ColorF(red, green, blue);
		}

		/// <summary>
		/// 设置字符消息的处理函数
		/// </summary>
		/// <param name="charCallback"></param>
		void SetKeyCallback(WindCallback::CharInputCallback_D2D charCallback)
		{
			m_CharCallback = charCallback;
		}
		/// <summary>
		/// 设置窗口绘制过程函数
		/// </summary>
		/// <param name="paintCallback"></param>
		void SetPaintCallback(WindCallback::PaintCallback_D2D paintCallback)
		{
			m_PaintCallback = paintCallback;
		}
		/// <summary>
		/// 窗口大小改变时调用
		/// </summary>
		/// <param name="sizeCallback"></param>
		void SetWindSizeCallback(WindCallback::SizeCallback_D2D sizeCallback)
		{
			m_WindSizeCallback = sizeCallback;
		}
		bool DrawRectangle(
			int left,
			int top,
			int right,
			int bottom)override
		{
			if(m_d2dRenderTarget)
			{
				D2D1_RECT_F rect = D2D1::RectF(left, top, right, bottom);
				m_d2dRenderTarget->FillRectangle(rect, m_PenBrush);
				return true;
			}
			return false;
		}
		bool ClearWindBackground(COLORREF color = RGB(255, 255, 255))override
		{
			if (!m_d2dRenderTarget)
				return false;
			m_d2dRenderTarget->Clear(D2D1::ColorF(color));
			return true;
		}
		bool AutoClearWindBackground(COLORREF color = RGB(255, 255, 255), bool run = true)override
		{
			SafeReleaseP(&m_BackgroundColor);
			if (!run)
			{
				return true;
			}
			m_BackgroundColor = new COLORREF(color);
			if (m_BackgroundColor)
				return true;
			return false;
		}
		bool DrawLine(int startX, int startY, int endX, int endY)override
		{
			if (!m_d2dRenderTarget)
				return false;
			m_LastPoint = D2D1::Point2F(endX, endY);
			m_d2dRenderTarget->DrawLine(D2D1::Point2F(startX, startY), m_LastPoint, m_PenBrush);
			return true;
		}
		bool DrawLineTo(int toX, int toY)override
		{
			if (!m_d2dRenderTarget)
				return false;
			m_d2dRenderTarget->DrawLine(m_LastPoint, D2D1::Point2F(toX, toY), m_PenBrush);
			return true;
		}
		bool DrawTextAscii(const std::string& ShowText, int x, int y, int w, int h)override
		{
			return false;
		}
		bool DrawTextWide(CQSTR ShowText, int x, int y, int w, int h)override
		{
			if (!m_d2dRenderTarget)
				return false;
			auto rect = D2D1::RectF(x, y, x + w, y + h);
			m_d2dRenderTarget->DrawTextW(ShowText.c_str(), ShowText.size(), m_TextFormat, rect, m_PenBrush);
			return true;
		}

		float GetPaintIntervalTime()const
		{
			return m_IntervalTime;
		}

		bool SetPen(COLORREF color, PenStyle penStyle = PenStyle::SolidLine, int LineWidth = 1)override
		{
			D2D1_STROKE_STYLE_PROPERTIES d2dPenStyle = D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,   // 端点样式
				D2D1_CAP_STYLE_FLAT,   // 连接处样式
				D2D1_CAP_STYLE_FLAT,   // 虚线起点端点样式
				D2D1_LINE_JOIN_BEVEL,  // 虚线样式
				10.0f,                  // 虚线间隔
				(D2D1_DASH_STYLE)penStyle);  // 实线样式
			m_PenWidth = LineWidth;
			if (!SetPenColor(color))
				return false;
			if (!SetPenStyle(d2dPenStyle, {}))
				return false;
			return true;
		}
		bool SetPenStyle(const D2D1_STROKE_STYLE_PROPERTIES& penStyle, const std::vector<float>dashes)
		{
			SafeRelease(&m_PenStyle);
			auto hr = m_d2dFactory->CreateStrokeStyle(penStyle,
				dashes.data(),
				dashes.size(),
				&m_PenStyle);
			if (FAILED(hr))
				return false;
			return true;
		}
		bool SetPenColor(D2D1::ColorF color)
		{
			SafeRelease(&m_PenBrush);
			if ((m_d2dRenderTarget->CreateSolidColorBrush(color, &m_PenBrush))<0)
			{
				return false;
			}
			return true;
		}
		bool SetPenColor(COLORREF color)
		{
			return SetPenColor(GdiColorToD2DColor(color));
		}
	};


}