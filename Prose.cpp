#include "pch.h"
#include "App.h"

ProseApp   *App;
MainWnd    *Wnd;

std::map<int, String> Languages;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
 UNREFERENCED_PARAMETER(hPrevInstance);
 UNREFERENCED_PARAMETER(lpCmdLine);

 HRESULT hr;
 int ret=0;

 hr=::CoInitialize(NULL);
 if (FAILED(hr))
   return 0;

 App=new ProseApp();

 if (App->Init(hInstance, nCmdShow, L"Prose", L"Windows Program Prose")==true)
  {
   ret = App->GetMain()->Loop(IDC_PROSE);
   App->Shutdown();
  }

 ::CoUninitialize();
 return ret;
}

