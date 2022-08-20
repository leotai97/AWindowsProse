#pragma once

#include <AWindowsLib.h>

#include "resource.h"

#include "Dialogs.h"
#include "MainWnd.h"

class ProseApp: public Application
{
 public:
 
 ProseApp();
~ProseApp();

 virtual bool Init(HINSTANCE hInst, int nCmdShow, String const &app, String const &displayName);

 ProseUnit Prose;

 const ProseLanguage English = ProseLanguage(1, L"English");
};

extern ProseApp  *App;
extern MainWnd   *Wnd;

