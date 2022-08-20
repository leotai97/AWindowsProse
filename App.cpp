#include "pch.h"
#include "App.h"

ProseApp::ProseApp()
{
}

ProseApp::~ProseApp()
{
}

bool ProseApp::Init(HINSTANCE hInst, int nCmdShow, String const &appName, String const &displayName)
{
 bool ret;

 if (Application::Init(hInst, nCmdShow, appName, displayName) == false)
   return false;

 MainWnd::Register(L"MainWndClass");

 Wnd=new MainWnd();
 m_Main = (PopUpWnd *)Wnd;
 ret=m_Main->Create(L"MainWndClass", SW_MAXIMIZE); // create main app window

 Prose.Clear();

 return ret; 
}
