#pragma once

#define ID_FILE         2001
#define ID_FILE_NEW     2002 
#define ID_FILE_OPEN    2003
#define ID_FILE_SAVE    2004
#define ID_FILE_EXIT    2005
#define ID_EDIT         2006
#define ID_EDIT_ADD    2007
#define ID_EDIT_EDIT   2008
#define ID_EDIT_DROP   2009
#define ID_EDIT_TYPE   2021
#define ID_EDIT_LANG_ADD 2010
#define ID_EDIT_LANG_DROP 2011
#define ID_EDIT_PASTE     2012
#define ID_EDIT_EXTRACT   2013
#define ID_VIEW           2014
#define ID_VIEW_SINGLE_MODE 2015
#define ID_VIEW_DUAL_MODE   2016
#define ID_VIEW_SET_LANGUAGES 2017
#define ID_VIEW_REFRESH_LIST 2018
#define ID_HELP              2019
#define ID_HELP_ABOUT        2020

class MainWnd : public PopUpWnd
{
 public:

 MainWnd();
~MainWnd();

 static void Register(String const &wndcls);
 virtual bool Create(String const &wndcls, int nCmdShow);
 virtual Rect ClientRect(); 

 protected:

 void LoadMainMenu();

 void RefreshList();
 void RefreshSingleMode();
 void RefreshDualMode();
 void OnColumnClick(int col);
 void OnDoubleClick(NMITEMACTIVATE *nia);
 bool ChanceToSave();

 static int ListCompare(LPARAM lvx, LPARAM lvy, LPARAM nAscDesCol);

 virtual void OnPaint();
 virtual void OnSize();
 virtual WMR OnNotify(HWND hWnd, int child, UINT code, LPARAM lParam);
 virtual bool OnClosing();  
 virtual WMR MenuHandler(int id);
 

 void mnuFileNew();
 void mnuFileOpen();
 void mnuFileSave();
 void mnuFileExit();
 void mnuEditAdd();
 void mnuEditEdit();
 void mnuEditDrop();
 void mnuEditTypeChange();
 void mnuEditLangAdd();
 void mnuEditLangDrop();
 void mnuEditPaste();
 void mnuEditExtract();
 void mnuViewSingleMode();
 void mnuViewDualMode();
 void mnuViewSetLanguages();
 void mnuViewRefreshList();
 void mnuHelpAbout();

 ToolBar     m_ToolBar;
 ListView    m_List;
 StatusBar   m_Status;     

 bool m_SingleMode;

 ProseLanguage m_LanguageKnown;
 ProseLanguage m_LanguageTarget;

 int   m_SortColumn;
 bool  m_AscDesc;

 bool  m_Changed;

 ProseTextItem::TextItemType m_LastType;

 AMenu m_Menu;
};

