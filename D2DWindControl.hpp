#pragma once
#include"WindowElements.hpp"
#include<map>

namespace Game {
	class d2dImageButton
	{
		WindControl::d2dClickDetection m_CheckRect;
		WindControl::d2dPicture m_BackgroundImage;
		WindControl::d2dText m_ShowText;
	public:
		d2dImageButton(){}
		void Init(float x, float y, float w, float h, const std::wstring& str,const D2D1_COLOR_F& strColor, WindCallback::ButtonCallback callback, MainWind_D2D* TargetWind, const std::wstring& backgroundImageFilePath = L"")
		{
			
			m_ShowText.SetPosition(x, y);
			m_ShowText.SetShowWide(w, h);
			m_ShowText.SetShowText(str);
			m_ShowText.SetColor(strColor,TargetWind->GetD2DTargetP());
			m_CheckRect.SetPosition(x, y);
			m_CheckRect.SetWide(w, h);
			m_CheckRect.SetButtonCallback(callback);
			if (!backgroundImageFilePath.empty())
			{
				m_BackgroundImage.LoadPictureFromFile(backgroundImageFilePath, TargetWind);
				m_BackgroundImage.SetPosition(x, y);
				m_BackgroundImage.SetShowWide(w, h);
			}
		}
		void Bind(MainWind_D2D* TargetWind)
		{
			TargetWind->AddButten(&m_CheckRect);
		}
		void Unbind(MainWind_D2D* TargetWind)
		{
			TargetWind->DeleteButten(&m_CheckRect);
		}
		void Draw(MainWind_D2D* wind)
		{
			m_BackgroundImage.Draw(wind);
			m_ShowText.Draw(wind);
		}
		void LoadBackgroundImage(const std::wstring& filePath, MainWind_D2D* TargetWind)
		{
			m_BackgroundImage.LoadPictureFromFile(filePath, TargetWind);
		}
		void SetCallback(WindCallback::ButtonCallback cb)
		{
			m_CheckRect.SetButtonCallback(cb);
		}
		void LinkWindow(MainWind_D2D* wind)
		{
			wind->AddButten(&m_CheckRect);
		}
		void DisconnectWindow(MainWind_D2D* wind)
		{
			wind->DeleteButten(&m_CheckRect);
		}
		void SetShowText(const std::wstring& str)
		{
			m_ShowText.SetShowText(str);
		}
	};
	class d2dColorButton
	{
		WindControl::d2dClickDetection m_CheckRect;
		WindControl::d2dRectangle m_Rectangle;
		WindControl::d2dText m_ShowText;
	public:
		d2dColorButton(){}
		void Init(float x, float y, float w, float h, 
			const std::wstring& str, 
			const D2D1_COLOR_F& textColor,const D2D1_COLOR_F& bkColor,
			WindCallback::ButtonCallback callback, MainWind_D2D* TargetWind)
		{
			D2D_RECT_F rect = { x,y,x + w,y + h };
			SetRect(rect);
			m_CheckRect.SetButtonCallback(callback);
			m_ShowText.SetColor(textColor,TargetWind->GetD2DTargetP());
			m_ShowText.SetShowText(str);
			m_Rectangle.SetColor(bkColor,TargetWind->GetD2DTargetP());
			m_Rectangle.FillRect();
		}
		void Bind(MainWind_D2D* TargetWind)
		{
			TargetWind->AddButten(&m_CheckRect);
		}
		void Unbind(MainWind_D2D* TargetWind)
		{
			TargetWind->DeleteButten(&m_CheckRect);
		}
		void SetRect(const D2D1_RECT_F& rect)
		{
			m_CheckRect.SetRectangle(rect);
			m_Rectangle.SetShowRect(rect);
			m_ShowText.SetShowRect(rect);
		}
		void Draw(MainWind_D2D* wind)
		{
			m_Rectangle.Draw(wind);
			m_ShowText.Draw(wind);
		}
		void SetCallback(WindCallback::ButtonCallback cb)
		{
			m_CheckRect.SetButtonCallback(cb);
		}
		void LinkWindow(MainWind_D2D* wind)
		{
			wind->AddButten(&m_CheckRect);
		}
		void DisconnectWindow(MainWind_D2D* wind)
		{
			wind->DeleteButten(&m_CheckRect);
		}
		void SetShowText(const std::wstring& str)
		{
			m_ShowText.SetShowText(str);
		}
	};

	/// <summary>
	///  用于显示函数图像
	/// </summary>
	class d2dFunctionWind
	{
		//x(min-max)y(min-max)
		D2D1_RECT_F m_Coordinate;
		D2D1_RECT_F m_ShowRect;
		std::map<int, WindControl::d2dFoldLine>m_Data;
	public:
		d2dFunctionWind(){}
		void SetShowRect(const D2D1_RECT_F& rect)
		{
			m_ShowRect = rect;
		}
		void SetCoordinate(float minX, float maxX, float minY, float maxY)
		{
			m_Coordinate = { minX,maxX,minY,maxY };
		}
		int AddFunction(const std::vector<float>& xList, const std::vector<float>& yList,int id,const D2D1_COLOR_F& color,ID2D1HwndRenderTarget* tar)
		{
			if (xList.size() != yList.size() || id < 0)
			{
				std::cout << "列表数量不匹配" << std::endl;
				return -1;
			}
			auto&& fl = WindControl::d2dFoldLine();
			m_Data[id] = (fl);
			fl.SetShowRect(m_ShowRect);
			fl.SetFoldLine(xList, yList);
			fl.SetColor(color, tar);
			return id;
		}
		bool DeleteFunction(int id)
		{
			auto result = m_Data.find(id);
			if (result == m_Data.end())
				return false;
			m_Data.erase(result);
			return true;;
		}
		WindControl::d2dFoldLine* GetFunction(int id)
		{
			auto result = m_Data.find(id);
			if (result == m_Data.end())
				return nullptr;
			return &result->second;
		}
		void Draw(ID2D1HwndRenderTarget* target)
		{
			target->PushAxisAlignedClip(m_ShowRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			for (auto& fl : m_Data)
			{
				fl.second.Draw(target);
			}
			target->PopAxisAlignedClip();
		}
	};
}