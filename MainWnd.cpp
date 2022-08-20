#include "pch.h"
#include "app.h"

MainWnd::MainWnd()
{
 m_SingleMode = true;
 m_SortColumn = 0;
 m_AscDesc = true;
 m_LastType = ProseTextItem::TextItemType::ResourceID;
}

MainWnd::~MainWnd()
{
}

void MainWnd::Register(String const &wndcls)
{
 WNDCLASSEX wcex={0};

 wcex.cbSize = sizeof(WNDCLASSEX);
 wcex.style          = CS_HREDRAW | CS_VREDRAW;
 wcex.lpfnWndProc    = PopUpWnd::PopUpWndProc;
 wcex.cbClsExtra     = 0;
 wcex.cbWndExtra     = 0;
 wcex.hInstance      = App->Instance();
 wcex.hIcon          = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PROSE));
 wcex.hCursor        = LoadCursor(0, IDC_ARROW);
 wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
 wcex.lpszMenuName   = 0;
 wcex.lpszClassName  = wndcls.Chars();
 wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

 if (::RegisterClassEx(&wcex)==0)
   throw L"Register failed";

 App->AddCustomClass(wndcls);
}

bool MainWnd::Create(String const &wndcls, int nCmdShow)
{
 String s;
 DWORD xStyle, fStyle;

 if (PopUpWnd::Create(wndcls, nCmdShow)==false)
   return false;

 m_ToolBar.Create(this, 3);
 m_ToolBar.AddItem(IDB_ADD, IDB_ADD, L"Add Text Item");
 m_ToolBar.AddItem(IDB_EDIT, IDB_EDIT, L"Edit Text Item");
 m_ToolBar.AddItem(IDB_DROP, IDB_DROP, L"Drop Text Item");
 
 xStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER;
 fStyle = LVS_REPORT | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS; 
 m_List.Create(this, xStyle, fStyle, Rect(0,0,10,10));
 m_List.SetOwner(this);
 m_List.AddTextColumn(L"Text ID", ListViewColumn::ColumnAlign::Left, 100);
 m_List.AddTextColumn(L"Type", ListViewColumn::ColumnAlign::Left, 50);
 m_List.AddTextColumn(L"English", ListViewColumn::ColumnAlign::Left, 50);

 m_Status.AddAutoPane(StatusBarPane::Content::Text);
 m_Status.AddFixedPane(StatusBarPane::Content::Progress, 190);
 m_Status.Create(this);

 OnSize();

 LoadMainMenu();

 SetMenu(m_Menu);

 return true;
}

void MainWnd::LoadMainMenu()
{
 MenuItem *sub;

 m_Menu.CreateWindowMenu();

 sub = m_Menu.SubMenu(ID_FILE, L"&File");
 sub->AddMenu(ID_FILE_NEW, L"New Project");
 sub->AddMenu(ID_FILE_OPEN, L"Open Project");
 sub->AddMenu(ID_FILE_SAVE, L"Save Project");
 sub->Separator();
 sub->AddMenu(ID_FILE_EXIT, L"Exit");

 sub = m_Menu.SubMenu(ID_EDIT, L"Edit");
 sub->AddMenu(ID_EDIT_ADD, L"Add Text Item");
 sub->AddMenu(ID_EDIT_EDIT, L"Edit Text Item");
 sub->AddMenu(ID_EDIT_DROP, L"Drop Text Item");
 sub->AddMenu(ID_EDIT_TYPE, L"Change ID Type");
 sub->Separator();
 sub->AddMenu(ID_EDIT_LANG_ADD, L"Add Language");
 sub->AddMenu(ID_EDIT_LANG_DROP, L"Drop Language");
 sub->Separator();
 sub->AddMenu(ID_EDIT_PASTE, L"Paste Resource ID List");
 sub->AddMenu(ID_EDIT_EXTRACT, L"Extract Text From .RC File");

 sub = m_Menu.SubMenu(ID_HELP, L"Help");
 sub->AddMenu(ID_HELP_ABOUT, L"About Program");

}

Rect MainWnd::ClientRect()
{
 RECT r;
 Rect rct;
 int h;

 if (m_Status.Handle() == 0) 
   return PopUpWnd::ClientRect(); 

 h=m_ToolBar.Height() + m_Status.Height();

 ::GetClientRect(m_hWnd, &r);
 rct=Rect(0, 0, r.right-2, r.bottom-( 2 + h) ); 
  
 return rct;
}

void MainWnd::OnPaint()
{
 PAINTSTRUCT ps;
 RECT rct;
 HDC hDC;
 
 hDC = ::BeginPaint(m_hWnd, &ps);
  {
   ::GetClientRect(m_hWnd, &rct);
   ::FillRect(hDC, &rct, ::GetSysColorBrush(COLOR_WINDOW));
   ::FrameRect(hDC, &rct, ::GetSysColorBrush(COLOR_WINDOW));
  }
 ::EndPaint(m_hWnd, &ps);
}

void MainWnd::OnSize()
{
 Rect r;
 Size cs;

 if (Minimized() == false && m_Status.Handle() != 0)
  {
   cs = ClientSize();
   m_ToolBar.SetRect(Rect(0,0, cs.Width, m_ToolBar.Height()));
   m_Status.OnSize(Rect(0,0, cs.Width,cs.Height));
   r = Rect(0, m_ToolBar.Height(), cs.Width, cs.Height - (m_ToolBar.Height() + m_Status.Height()));
   m_List.SetRect(r);
  }
}

void MainWnd::RefreshList()  // any lang, any lang
{
 if (m_SingleMode == true)
  {
   RefreshSingleMode();
  }
 else
  {
   RefreshDualMode();
  }
}

void MainWnd::RefreshSingleMode() // english
{
 ListViewItem item;
 String sid, txt;

 m_List.Clear();

 for(auto &li : App->Prose.Items)
  {
   sid = li.second.Key;
   item = ListViewItem(sid, 0, 0);
   if (li.second.ItemType == ProseTextItem::TextItemType::ResourceID)
     txt = L"Resource ID";
   else
     txt = L"Prose ID";
   item.SubItems.push_back(txt);
   if (li.second.LanguageText.count(App->English.ID) > 0) 
     txt = li.second.LanguageText[App->English.ID];
   else
     txt.Clear();
   item.SubItems.push_back(txt);
   m_List.Insert(item);
  }
 m_List.AutoSize(0, ListView::AutoSizes::Content);
 m_List.AutoSize(1, ListView::AutoSizes::Content);
 m_List.AutoSize(2, ListView::AutoSizes::Content);
 ListView_SortItemsEx(m_List.Handle(), MainWnd::ListCompare, Utility::AscendDescendColumn(m_AscDesc, m_SortColumn));
}

void MainWnd::RefreshDualMode()
{
 ListViewItem item;
 String sid, txt;

 m_List.Clear();

 for(auto &li : App->Prose.Items)
  {
   sid = li.second.Key;
   item = ListViewItem(sid, 0, 0);
   if (li.second.ItemType == ProseTextItem::TextItemType::ResourceID)
     txt = L"Resource ID";
   else
     txt = L"Prose ID";
   item.SubItems.push_back(txt);
   if (li.second.LanguageText.count(m_LanguageKnown.ID) > 0) 
     txt = li.second.LanguageText[m_LanguageKnown.ID];
   else
     txt.Clear();
   item.SubItems.push_back(txt);
   if (li.second.LanguageText.count(m_LanguageTarget.ID) > 0) 
     txt = li.second.LanguageText[m_LanguageTarget.ID];
   else
     txt.Clear();
   item.SubItems.push_back(txt);
   m_List.Insert(item);
  }
 m_List.AutoSize(0, ListView::AutoSizes::Content);
 m_List.AutoSize(1, ListView::AutoSizes::Content);
 m_List.AutoSize(2, ListView::AutoSizes::Content);
 m_List.AutoSize(3, ListView::AutoSizes::Content);
 ListView_SortItemsEx(m_List.Handle(), MainWnd::ListCompare, Utility::AscendDescendColumn(m_AscDesc, m_SortColumn));
}

int MainWnd::ListCompare(LPARAM lvx, LPARAM lvy, LPARAM nAscDesCol)
{
 bool AscDesc;
 int nCol;
 String ndx, ndy;

 if ((nAscDesCol & 0xFF00) > 0) AscDesc = true; else AscDesc=false;
 nCol = (int)(nAscDesCol & 0x00FF);
 if (AscDesc==true)
  {
   ndx = Wnd->m_List.GetItemText(lvx, nCol);
   ndy = Wnd->m_List.GetItemText(lvy, nCol);
  }
 else
  {
   ndx = Wnd->m_List.GetItemText(lvy, nCol);
   ndy = Wnd->m_List.GetItemText(lvx, nCol);
  }
 return String::Compare(ndx, ndy);
}

void MainWnd::OnColumnClick(int col)
{
 if (m_SortColumn == col)
   m_AscDesc = !m_AscDesc;
 else
  {
   m_SortColumn = col;
   m_AscDesc = true;
  }
 ListView_SortItemsEx(m_List.Handle(), MainWnd::ListCompare, Utility::AscendDescendColumn(m_AscDesc, m_SortColumn));
}

void MainWnd::OnDoubleClick(NMITEMACTIVATE *nia)
{
 mnuEditEdit();
}

WMR MainWnd::OnNotify(HWND hWnd, int child, UINT code, LPARAM lParam)
{
 switch(code)
  {
// case LVN_ITEMCHANGED: OnListViewItemChanged(child, (NMLISTVIEW *)lParam); break;
   case LVN_COLUMNCLICK: OnColumnClick(((NMLISTVIEW *)lParam)->iSubItem); break;
   case NM_DBLCLK:       OnDoubleClick((NMITEMACTIVATE *)lParam); break;
  }
 return WMR::Zero;
}

bool MainWnd::OnClosing()
{
 if (m_Changed == true)
   return ChanceToSave();
 else
   return true;
}

bool MainWnd::ChanceToSave() // return true and program will close or prose is overwritten etc
{
 DialogResult r;
 String msg, path;

 msg = L"Prose Unit ";
 msg += App->Prose.ProjectPath();
 msg +=L" has changed, do you want to save it?";
 r = App->Question(msg, MB_YESNOCANCEL);
 switch(r)
  {
   case DialogResult::Yes:
    {
     if (App->Prose.ProjectPath().Length() == 0)
      {
       path = OpenFileDlg::ChooseFolder(this->Handle(), L"Project Path");
       if (path.Length() == 0)
         return false;
       App->Prose.SetProjectPath(path);
      }
     if (App->Prose.Save() == false)
      {
       App->Response(App->Prose.Error());
       return false;
      }
     else
      {
       m_Status.SetText(0, L"Project Saved");
       m_Changed = false;
       return true;
      }
    } 
   case DialogResult::No:
    {
     return true;
    } 
   case DialogResult::Cancel:
    {
     return false;
    } 
  }
 return true;
}

WMR MainWnd::MenuHandler(int id)
{
 switch(id)
  {
   case ID_FILE_NEW:           mnuFileNew(); break;
   case ID_FILE_OPEN:          mnuFileOpen(); break;
   case ID_FILE_SAVE:          mnuFileSave(); break;
   case ID_FILE_EXIT:          mnuFileExit(); break;
   case IDB_ADD:               mnuEditAdd(); break;
   case IDB_EDIT:              mnuEditEdit(); break;
   case IDB_DROP:              mnuEditDrop(); break;
   case ID_EDIT_TYPE:          mnuEditTypeChange(); break;
   case ID_EDIT_LANG_ADD:      mnuEditLangAdd(); break;
   case ID_EDIT_LANG_DROP:     mnuEditLangDrop(); break;
   case ID_EDIT_PASTE:         mnuEditPaste(); break;
   case ID_EDIT_EXTRACT:       mnuEditExtract(); break;
   case ID_VIEW_SINGLE_MODE:   mnuViewSingleMode(); break;
   case ID_VIEW_DUAL_MODE:     mnuViewDualMode(); break;
   case ID_VIEW_SET_LANGUAGES: mnuViewSetLanguages(); break;
   case ID_VIEW_REFRESH_LIST:  mnuViewRefreshList(); break;
   case ID_HELP_ABOUT:         mnuHelpAbout(); break;
  }
 return WMR::One;
}

void MainWnd::mnuFileNew()
{
 String path, msg;

 if (m_Changed == true)
  {
   if (ChanceToSave() == false)
     return;
  }
 App->Prose.Clear();
 RefreshList();
}

void MainWnd::mnuFileOpen()
{
 String path, msg;
 bool ret;

 if (m_Changed == true)
  {
   if (ChanceToSave() == false)
     return;
  }

 App->Prose.Clear();
 m_Changed = false;

 path = OpenFileDlg::ChooseFolder(m_hWnd, L"Open c++ Project Folder");
 if (path.Length() == 0)
   return;
 
 ret = App->Prose.OpenProse(path);
 if (ret == false)
  {
   App->Response(App->Prose.Error());
   return;
  }

 mnuViewSingleMode();

 msg = L"Prose Project -- ";
 msg += App->Prose.ProjectPath();
 SetText(msg);
 m_Status.SetText(0, L"Project Opened");
}

void MainWnd::mnuFileSave()
{
 String path, msg;

 if (App->Prose.ProjectPath().Length() == 0)
  {
   path = OpenFileDlg::ChooseFolder(this->Handle(), L"Choose c++ Project Folder");
   if (path.Length()==0)
    {
     App->Response(L"Path not set. Save Cancelled");
     return;
    }
   App->Prose.SetProjectPath(path);
   msg = L"Prose Project -- ";
   msg += path; 
   SetText(msg); 
  }

 if (App->Prose.Save() == false)
  {
   App->Response(App->Prose.Error());
  }
 else
  {
   m_Status.SetText(0, L"Project Saved");
   m_Changed = false;
  }
}

void MainWnd::mnuFileExit()
{
 Close(); // OnClosing will handle saving if modified
}

void MainWnd::mnuEditAdd()
{
 TextItemDlg dlg;
 ProseTextItem item;
 DialogResult r;

 r = dlg.Show(this, App->English, m_LastType); 
 if (r == DialogResult::OK)
  {
   item = ProseTextItem(dlg.GetKey(), dlg.GetType());
   item.LanguageText.insert(std::pair<int, String>(App->English.ID, dlg.GetTextEntry()));
   App->Prose.Items.insert(std::pair<String, ProseTextItem>(item.Key, item));
   RefreshList();
   m_Changed = true;
   m_Status.SetText(0, L"Item Added");
   m_LastType = item.ItemType;
  }
}

void MainWnd::mnuEditEdit()
{
 DualItemDlg dlgDual;
 TextItemDlg dlgSing;
 ProseTextItem item;
 DialogResult r;
 String key, txt;
 int ndx;

 ndx = m_List.GetSelectedItem();
 if (ndx < 0)
   return;

 key = m_List.GetItemText(ndx, 0);

 if (App->Prose.Items.count(key) == 0)
   throw L"key not found in ProseUnit";

 item = App->Prose.Items[key];

 if (m_SingleMode == true)
  {
   r = dlgSing.Show(this, App->English, item);
  }
 else
  {
   r = dlgDual.Show(this, m_LanguageKnown, m_LanguageTarget, item);
  }
 if (r == DialogResult::OK)
  {
   item = App->Prose.Items[key];  // get updated copy of item
   m_List.SetItemText(ndx, 0, item.Key);
   if (item.ItemType == ProseTextItem::TextItemType::ResourceID)
     txt = L"Resource ID";
   else
     txt = L"Prose ID";
   if (m_SingleMode == true)
    {
     if (item.LanguageText.count(App->English.ID) > 0) 
       txt = item.LanguageText[App->English.ID];
     else
       txt.Clear();
     m_List.SetItemText(ndx, 2, txt);
    }
   else
    {
     if (item.LanguageText.count(m_LanguageKnown.ID) > 0) 
       txt = item.LanguageText[m_LanguageKnown.ID];
     else
       txt.Clear();
     m_List.SetItemText(ndx, 2, txt);
     if (item.LanguageText.count(m_LanguageTarget.ID) > 0) 
       txt = item.LanguageText[m_LanguageTarget.ID];
     else
       txt.Clear();
     m_List.SetItemText(ndx, 3, txt);
    }
   m_Changed = true;
   m_Status.SetText(0, L"Item Edited");
  }
}

void MainWnd::mnuEditDrop()
{
 ProseTextItem item;
 String key, msg;
 std::vector<int> indices;

 indices = m_List.GetSelectedIndices();
 if (indices.size() == 0)
  {
   App->Response(L"No items selected to delete");
   return;
  }

 msg = L"Delete ";
 if (indices.size() > 1)
  {
   msg += String::Decimal((int)indices.size());
   msg += L" Selected Items ?";
  }
 else
  {
   msg += m_List.GetItemText(indices[0], 0);
  }
 if (App->Question(msg, MB_OKCANCEL) != DialogResult::OK)
   return;

 for(const auto &ndx : indices)
  {
   key = m_List.GetItemText(ndx, 0);
   if (App->Prose.Items.count(key) == 0)
     throw L"key not found in ProseUnit";
   App->Prose.Items.erase(key);
  }
 
 m_List.RemoveItems(indices);
 m_Status.SetText(0, L"Items Dropped");
 m_Changed = true;
}

void MainWnd::mnuEditTypeChange()
{
 TextTypeDlg dlg;
 std::vector<int> indices;
 String key;

 indices = m_List.GetSelectedIndices();

 if (indices.size() == 0)
  {
   App->Response(L"nothing selected");
   return;
  }

 if (dlg.Show(this) != DialogResult::OK)
   return;

 for(const auto &ndx : indices)
  {
   key = m_List.GetItemText(ndx, 0);
   if (App->Prose.Items.count(key) == 0) throw L"key not found";
   App->Prose.Items[key].ItemType = dlg.TextType();
  }
 RefreshList();
}

void MainWnd::mnuEditLangAdd()
{
 ProseLanguage::LanguageOrientation orient;
 LanguageDlg dlg;
 String lang;
 int id;

 if (dlg.Show(this) == DialogResult::OK)
  {
   id = App->Prose.Languages.size()+1;
   orient = dlg.Orientation();
   lang = dlg.LanguageName();
   App->Prose.Languages.insert(std::pair<int, ProseLanguage>(id, ProseLanguage(id, orient, lang)));
   m_Status.SetText(0, L"Language Added");
  }
}

void MainWnd::mnuEditLangDrop()
{
 LanguageChooseDlg dlg;
 ProseLanguage lang;
 String msg;

 if (dlg.Show(this, LanguageChooseDlg::ChooseType::Drop) == DialogResult::OK)
  {
   lang = dlg.Language();
   if (m_SingleMode == true)
    {
     if (lang.ID == App->English.ID)
      {
       App->Response(L"English cannot be removed");
       return;
      }
    }
   else
    {
     if (m_LanguageKnown.ID == lang.ID || m_LanguageTarget.ID == lang.ID)
      {
       App->Response(L"Can't delete the current known or target languages, switch to single English mode or change the known and target languages to something else");
       return;
      }
    }
   for (auto &item : App->Prose.Items)
    { 
     if (item.second.LanguageText.count(lang.ID) > 0)
       item.second.LanguageText.erase(lang.ID);
    }
   App->Prose.Languages.erase(lang.ID);
   RefreshList();
   m_Status.SetText(0, L"Language Dropped");
  }
}

void MainWnd::mnuEditPaste()
{
 ProseTextItem proseItem;
 std::vector<String> items;
 std::vector<String> ids;
 String text, part, id, line;
 String msg, msgFinal;
 int start, end;
 int msgGood, msgBad;

 msgGood = 0;
 msgBad = 0;
 text = App->GetClipboardText();
 if (text.Length() == 0)
  {
   App->Response(L"The clipboard is empty");
   return;
  }

 items = text.Split(L"\r\n");

 if (items.size() == 0)
  {
   App->Response(L"Clipbard didn't contain any Resource lines");
   return;
  }

 for (const auto &item : items)
  {
   line = item.Trim();
   start = line.IndexOf(L" ");
   if (start < 7)
    {
     msg = L"Did not find space after #define in line \"";
     msg += line;
     msg += L"\"";
     App->Response(msg);
     return;
    }
   if (line.Substring(0, start) != L"#define")
    {
     msg = L"Line did not start with #define \"";
     msg += line;
     msg += L"\"";
     App->Response(msg);
     return;     
    }
   part = line.Substring(start).Trim();
   if (part.Length()==0)
    {
     msg = L"Line did not contain a resource ID after #define \"";
     msg += line;
     msg += L"\"";
     App->Response(msg);
     return;     
    }
   end = part.IndexOf(L"\t");
   if (end < 0)
     end = part.IndexOf(L" ");
   if (end < 0)
    {
     msg = L"Did not find TAB or space after Resource ID line \"";
     msg += line;
     msg += L"\"";
     App->Response(msg);
     return;
    }
   id = part.Substring(0, end).Trim();
   ids.push_back(id);
   if (App->Prose.Items.count(id) == 0)
     msgGood++;
   else
    {
     if (msgBad < 10)
      {
       msg +=L"\n";
       msg += id;
      }
     if (msgBad == 10)
       msg += L"\nList Cut Short";
     msgBad++;      
    }
  }

 if (msgGood == 0)
  {
   App->Response(L"No new Resource IDs found in clipboard");
   return;
  }

 if (msgBad>0)
  {
   msgFinal = String::Decimal(msgBad);
   msgFinal=L" Resource IDs have already been added:\n";
   msgFinal += msg;
   msgFinal += L"\n\n";
  }
 
 if (msgGood > 0)
  {
   msgFinal += String::Decimal(msgGood);
   msgFinal += L" Resource IDs are available to add";
  }

 msgFinal += L"\n\nAdd avaiable Resource IDs ?";

 if (App->Question(msgFinal, MB_OKCANCEL) == DialogResult::OK)
  {
   for (const auto &idc : ids)
    {
     if (App->Prose.Items.count(idc) == 0)
      {
       proseItem=ProseTextItem(idc, ProseTextItem::TextItemType::ResourceID);
       for(const auto &lang : App->Prose.Languages)
        {
         proseItem.LanguageText.insert(std::pair<int, String>(lang.second.ID, L""));
        }
       App->Prose.Items.insert(std::pair<String, ProseTextItem>(idc, proseItem));
      }
    }
   m_Changed=true;
   RefreshList();
  }
}

void MainWnd::mnuEditExtract()
{
 ProseLanguage lang;
 LanguageChooseDlg dlg;
 std::vector<String> lines;
 AFileTextIn file;
 String text, id, part1, part2, part3, part4;
 int p1, p2, p3, p4, p5;
 String path;
  
 path = OpenFileDlg::ChoooseFile(this->Handle(), App->Instance(), L"Open RC File", OpenFileDlg::FileType::RC);

 if (path.Length() == 0)
   return;

 if (file.Open(path) == false)
  {
   App->Response(file.Error());
   return;
  }

 lines = file.ReadAll();
 
 file.Close();

 if (lines.size() == 0)
  {
   App->Response(L"File was empty");
   return;
  }

 if (dlg.Show(this, LanguageChooseDlg::ChooseType::Select) != DialogResult::OK)
   return;

 // MENUITEM "New File",  ID_FILE_NEW

 lang = dlg.Language();

 for(const auto &line : lines)
  {
   p1 = line.IndexOf(L'M');
   if (p1 >= 0)
    {
     if (line.Substring(p1, 8) == L"MENUITEM")
      {
       part1 = line.Substring(p1 + 9);
       p2 = part1.IndexOf(L"\"");
       if (p2>=0)
        {
         part2 = part1.Substring(p2+1);
         p3 = part2.IndexOf(L"\"");
         text = part2.Substring(0, p3);
         part3 = part2.Substring(p3+2);
         p4 = part3.NotIndexOf(L' ');
         if (p4 >= 0)
          {
           part4 = part3.Substring(p4);
           p5 = part4.IndexOf(L' ');
           if (p5 > 0)
             id = part4.Substring(0, p5);          
           else
             id  = part4;
           if (App->Prose.Items.count(id) > 0)
            {
             App->Prose.Items[id].LanguageText[lang.ID] = text;
             if (m_Changed != true)
               m_Changed = true;
            }
          }
        }
      }
    }
  }
 if (m_Changed == true)
   RefreshList();
}

void MainWnd::mnuViewSingleMode()
{
 if (m_SingleMode == false)
  {
   m_SingleMode = true;
   m_Menu.SetCheckState(ID_VIEW_DUAL_MODE, false);
   m_Menu.SetCheckState(ID_VIEW_SINGLE_MODE, true);

   // 0 id  1 type 2 english

   m_List.DeleteColumn(3);
   m_List.DeleteColumn(2);
   m_List.AddTextColumn(L"English", ListViewColumn::ColumnAlign::Left, 400);
  }
 RefreshList();
}

void MainWnd::mnuViewDualMode()
{
 SetLanguageDlg dlg;

 if (m_SingleMode == false)
   return;

 if (dlg.Show(this) == DialogResult::OK)
  {
   m_LanguageKnown = dlg.KnownLanguage;
   m_LanguageTarget = dlg.TargetLanguage;
   m_SingleMode = false;
   m_Menu.SetCheckState(ID_VIEW_DUAL_MODE, true);
   m_Menu.SetCheckState(ID_VIEW_SINGLE_MODE, false);

   // 0 id  1 type  2 known 3 target

   m_List.DeleteColumn(2);

   m_List.AddTextColumn(m_LanguageKnown.Name, ListViewColumn::ColumnAlign::Left, 50);
   m_List.AddTextColumn(m_LanguageTarget.Name, ListViewColumn::ColumnAlign::Left, 50);
  }
 RefreshList();
}

void MainWnd::mnuViewSetLanguages()
{
 SetLanguageDlg dlg;

 if (App->Prose.Languages.size() < 2)
  {
   App->Response(L"Only one langage is defined");
   return;
  }

 if (dlg.Show(this) == DialogResult::OK)
  {
   m_LanguageKnown = dlg.KnownLanguage;
   m_LanguageTarget = dlg.TargetLanguage;
  }
}

void MainWnd::mnuViewRefreshList()
{
 RefreshList();
}

void MainWnd::mnuHelpAbout()
{
}

