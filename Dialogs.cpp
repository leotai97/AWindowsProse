#include "pch.h"
#include "app.h"

DialogResult TextItemDlg::Show(AWnd *parent, ProseLanguage const &lang, ProseTextItem const &ti)
{
 m_Item = ti;
 m_Language = lang;
 return ADialog::Show(IDD_TEXT_ITEM, parent);
}

DialogResult TextItemDlg::Show(AWnd *parent, ProseLanguage const &lang, ProseTextItem::TextItemType iType)
{
 m_Language = lang;
 m_ItemType = iType;
 return ADialog::Show(IDD_TEXT_ITEM, parent);
}

void TextItemDlg::OnInitDialog()
{
 String sid, txt, title;

 title = L"Text Item In ";
 title += m_Language.Name;
 SetText(title);

 if (m_Item.Key.Length() > 0)
  {
   SetItemText(IDC_TEXT_ITEM_ID, m_Item.Key);
   if (m_Item.LanguageText.count(m_Language.ID) == 0) 
     txt.Clear();
   else
     txt = m_Item.LanguageText[m_Language.ID];
   SetItemText(IDC_TEXT_ITEM_TEXT,txt);
   if (m_Item.ItemType == ProseTextItem::TextItemType::ResourceID)
    {
     SetCheckState(IDC_TEXT_ITEM_RID, true);
    }
   else
    {
     SetCheckState(IDC_TEXT_ITEM_PID, true);
    }
  }
 else
  {
   if (m_ItemType == ProseTextItem::TextItemType::ResourceID)
    {
     SetCheckState(IDC_TEXT_ITEM_RID, true);
    }
   else
    {
     SetCheckState(IDC_TEXT_ITEM_PID, true);
    }
  }
 ADialog::OnInitDialog();
}

WMR TextItemDlg::OnCommand(int child, HWND hChild)
{
 WMR ret = WMR::Zero;
 String msg, key;

 if (child == IDOK)
  {
   key = GetItemText(IDC_TEXT_ITEM_ID).Trim();
   m_Text = GetItemText(IDC_TEXT_ITEM_TEXT).Trim();
   if (GetCheckState(IDC_TEXT_ITEM_RID) == true)
     m_ItemType = ProseTextItem::TextItemType::ResourceID;
   else
     m_ItemType = ProseTextItem::TextItemType::ProseID;
   if (key.Length() == 0)
    {
     App->Response(L"Text ID cannot be blank");
     return ret;
    }
   if (m_Item.Key.Length() == 0)
    {
     for(const auto &sid : App->Prose.Keys)
      {
       if (sid.first == key)
        {
         msg = L"Text ID \"";
         msg += key;
         msg += L"Already Exists";
         App->Response(msg);
         return ret;
        }
      }
    }
   else
    {
     App->Prose.Items[m_Item.Key].ItemType = m_ItemType;
     App->Prose.Items[m_Item.Key].LanguageText[App->English.ID] = m_Text;
    }
   m_Key = key;
   Close(DialogResult::OK);
  }

 if (child == IDCANCEL)
   Close(DialogResult::Cancel);

 return ret;
}

// ////////////////////////////////////////////////////////////////////////

DialogResult DualItemDlg::Show(AWnd *parent, ProseLanguage const &langKnown, ProseLanguage const &targetLang, ProseTextItem const &ti)
{
 m_Item = ti;
 m_LanguageKnown = langKnown;
 m_LanguageTarget = targetLang;
 return ADialog::Show(IDD_TEXT_DUAL, parent);
}


void DualItemDlg::OnInitDialog()
{
 String sid, txt, title;

 title = L"Text Item From ";
 title += m_LanguageKnown.Name;
 title += L" To ";
 title += m_LanguageTarget.Name;
 SetText(title);

 m_Known.Attach(this, IDC_TEXT_ITEM_TEXT_1);
 m_Target.Attach(this, IDC_TEXT_ITEM_TEXT_2);
 
 m_Known.ReadOnly(true);
 if (m_LanguageKnown.Orientation == ProseLanguage::LanguageOrientation::Right)
   m_Known.RightAlign();

 if (m_LanguageTarget.Orientation == ProseLanguage::LanguageOrientation::Right)
   m_Known.RightAlign();

 if (m_Item.Key.Length() > 0)
  {
   SetItemText(IDC_TEXT_ITEM_ID, m_Item.Key);
   if (m_Item.LanguageText.count(m_LanguageKnown.ID) == 0) throw L"specific language not found";
   txt = m_Item.LanguageText[m_LanguageKnown.ID];
   SetItemText(IDC_TEXT_ITEM_TEXT_1,txt);
   if (m_Item.LanguageText.count(m_LanguageTarget.ID) == 0)
     txt.Clear();
   else 
     txt = m_Item.LanguageText[m_LanguageKnown.ID];
   SetItemText(IDC_TEXT_ITEM_TEXT_2,txt);
   if (m_Item.ItemType == ProseTextItem::TextItemType::ResourceID)
    {
     SetCheckState(IDC_TEXT_ITEM_RID, true);
    }
   else
    {
     SetCheckState(IDC_TEXT_ITEM_PID, true);
    }
  }
 else
  {
   SetCheckState(IDC_TEXT_ITEM_RID, true);
  }

 SetItemText(IDC_STATIC_LANG1, m_LanguageKnown.Name);
 SetItemText(IDC_STATIC_LANG2, m_LanguageTarget.Name);

 ADialog::OnInitDialog();
}

WMR DualItemDlg::OnCommand(int child, HWND hChild)
{
 WMR ret = WMR::Zero;
 String msg, key, txt;

 if (child == IDOK)
  {
   key = GetItemText(IDC_TEXT_ITEM_ID).Trim();
   m_Text = GetItemText(IDC_TEXT_ITEM_TEXT_2).Trim();
   if (GetCheckState(IDC_TEXT_ITEM_RID) == true)
     m_ItemType = ProseTextItem::TextItemType::ResourceID;
   else
     m_ItemType = ProseTextItem::TextItemType::ProseID;
   if (key.Length() == 0)
    {
     App->Response(L"Text ID cannot be blank");
     return ret;
    }
   if (m_Text.Length() == 0)
    {
     App->Response(L"Text ID cannot be blank");
     return ret;
    }
   if (String::Compare(key, m_Key) !=0)
    {
     for(const auto &sid : App->Prose.Keys)
      {
       if (sid.first == key)
        {
         msg = L"New value of Text ID \"";
         msg += key;
         msg += L"already exists";
         App->Response(msg);
         return ret;
        }
      }
     m_Key = key;
    }
   txt = GetItemText(IDC_TEXT_ITEM_TEXT_2).Trim(); // replace CRLF with " "
   if (App->Prose.Items[m_Key].LanguageText.count(m_LanguageTarget.ID) == 0)  
     App->Prose.Items[m_Key].LanguageText.insert(std::pair<int, String>(m_LanguageTarget.ID, txt));
   else
     App->Prose.Items[m_Key].LanguageText[m_LanguageTarget.ID] = txt;
   Close(DialogResult::OK);
  }

 if (child == IDCANCEL)
   Close(DialogResult::Cancel);

 return ret;
}

//////////////////////////////////////////////////////

DialogResult SetLanguageDlg::Show(AWnd *parent)
{
 return ADialog::Show(IDD_LANGUAGE_SET, parent);
}

void SetLanguageDlg::OnInitDialog()
{

 m_Known.Attach(this, IDC_SET_LANGUAGES_KNOWN);
 m_Target.Attach(this, IDC_SET_LANGUAGES_TARGET);

 for(const auto &lang : App->Prose.Languages)
  {
   m_Known.AddItem(ListItem(lang.second.Name, lang.second.ID));
  }
 for(const auto &lang : App->Prose.Languages)
  {
   m_Target.AddItem(ListItem(lang.second.Name, lang.second.ID));
  }

 if (KnownLanguage.ID > 0)
   m_Known.SetCurrentItem(KnownLanguage.ID);
 if (TargetLanguage.ID > 0)
   m_Target.SetCurrentItem(TargetLanguage.ID);

 ADialog::OnInitDialog();
}

WMR SetLanguageDlg::OnCommand(int child, HWND hChild)
{
 WMR ret = WMR::Zero;
 int kid, tid;

 if (child == IDOK)
  {
   if (m_Known.IsItemSelected() == false)
    {
     App->Response(L"Known Language Not Selected");
     return ret;
    }
   if (m_Target.IsItemSelected() == false)
    {
     App->Response(L"Target Language Not Selected");
     return ret;
    }
   kid = m_Known.GetSelectedTag();
   if (App->Prose.Languages.count(kid) == 0) throw L"Language id not found";
   KnownLanguage = App->Prose.Languages[kid];
   tid = m_Target.GetSelectedTag();
   if (tid == kid)
    {
     App->Response(L"You've got your known and target languages set to the same language");
     return ret;
    }
   if (App->Prose.Languages.count(tid) == 0) throw L"Language id not found";
   TargetLanguage = App->Prose.Languages[tid];
   Close(DialogResult::OK);
  }
 else
  {
   Close(DialogResult::Cancel);
  }
}

///////////////////////////////////////////////////////

DialogResult LanguageDlg::Show(AWnd *parent)
{
 m_LanguageName.Clear();
 m_ID = 0;
 return ADialog::Show(IDD_LANGUAGE, parent);
}

DialogResult LanguageDlg::Show(AWnd *parent, ProseLanguage const &lang)
{
 m_LanguageName = lang.Name;
 m_ID = lang.ID;
 m_Orient = lang.Orientation;
 return ADialog::Show(IDD_LANGUAGE, parent);
}

void LanguageDlg::OnInitDialog()
{
 if (m_LanguageName.Length() > 0)
  {
   SetItemText(IDC_LANGUAGE_EDIT, m_LanguageName); 
   if (m_Orient == ProseLanguage::LanguageOrientation::Left)
     SetCheckState(IDC_LANGUAGE_LEFT, true);
   else
     SetCheckState(IDC_LANGUAGE_RIGHT, true);
  }
 else
  {
   SetCheckState(IDC_LANGUAGE_LEFT, true);
  }
 ADialog::OnInitDialog();
}

WMR LanguageDlg::OnCommand(int child, HWND hChild)
{
 String lang, msg;
 WMR ret = WMR::Zero;

 if (child == IDOK)
  {
   if (GetCheckState(IDC_LANGUAGE_LEFT) == true)
     m_Orient = ProseLanguage::LanguageOrientation::Left;
   else
     m_Orient = ProseLanguage::LanguageOrientation::Right;
   lang = GetItemText(IDC_LANGUAGE_EDIT).Trim();
   if (lang.Length() == 0)
    {
     App->Response(L"Language name cannot be blank");
     return ret;
    }  
   m_LanguageName = lang; 
   if (m_ID != 0)
    {
     for(const auto &lng : App->Prose.Languages)
      {
       if (m_ID != lng.second.ID && String::Compare(lang, lng.second.Name) == 0)
        {
         msg = L"Change to the language name conflicts with the existing language, ";
         msg += lng.second.Name;
         App->Response(msg);
         return ret;
        }
      }
    }
   else
    {
     for (const auto &lng : App->Prose.Languages)
      {
       if (String::Compare(lang, lng.second.Name) == 0)
        {
         msg = L"Language name ";
         msg += lang;
         msg += L" already exists";
         App->Response(msg);
         return ret;
        }
      }
    }
   Close(DialogResult::OK);
  }
 if (child == IDCANCEL)
  {
   Close(DialogResult::Cancel);
  }
}

////////////////////////////////////////////////////

DialogResult LanguageChooseDlg::Show(AWnd *parent, ChooseType cType)
{
 m_Type = cType;
 return ADialog::Show(IDD_LANGUAGE_CHOOSE, parent);
}

void LanguageChooseDlg::OnInitDialog()
{
 ListViewItem item;

 switch(m_Type)
  {
   case ChooseType::Change: SetText(L"Choose Language To Change"); break;
   case ChooseType::Drop:   SetText(L"Choose Language To Delete"); break;
   case ChooseType::Select: SetText(L"Select Applicable Language"); break;
  }

 m_List.Attach(this, IDC_LANGUAGE_LIST);

 m_List.AddTextColumn(L"Language Name", ListViewColumn::ColumnAlign::Left, 200);
 m_List.AddTextColumn(L"Justification", ListViewColumn::ColumnAlign::Left, 200);
 m_List.AddTextColumn(L"ID #", ListViewColumn::ColumnAlign::Left, 20);

 for (const auto &lang : App->Prose.Languages)
  {
   item = ListViewItem(lang.second.Name, lang.second.ID);
   if (lang.second.Orientation == ProseLanguage::LanguageOrientation::Left)
     item.SubItems.push_back(L"Left");
   else
     item.SubItems.push_back(L"Right");
   item.SubItems.push_back(String::Decimal(lang.second.ID));
   m_List.Insert(item);
  }

 m_List.AutoSize(0, ListView::AutoSizes::Header);
 m_List.AutoSize(1, ListView::AutoSizes::Header);
 m_List.AutoSize(2, ListView::AutoSizes::Header);

 ADialog::OnInitDialog();
}

WMR LanguageChooseDlg::OnCommand(int child, HWND hChild)
{
 WMR ret = WMR::Zero;
 int ndx, id;

 if (child == IDOK)
  {
   if (m_List.SelectedItemsCount() == 0)
    {
     App->Response(L"Choose an item then click OK");
     return ret;
    }
   ndx = m_List.GetSelectedItem();
   id = m_List.GetItemParam(ndx);
   if (App->Prose.Languages.count(id) == 0) throw L"Item param not language id";
   m_Lang = App->Prose.Languages[id];
   Close(DialogResult::OK);
  }
 else
  {
   Close(DialogResult::Cancel);
  }

 return ret;
}

////////////////////////////////////////////////////

DialogResult TextTypeDlg::Show(AWnd *parent)
{
 return ADialog::Show(IDD_TEXT_TYPE, parent);
}

WMR TextTypeDlg::OnCommand(int child, HWND hChild)
{
 WMR ret = WMR::Zero;

 if (child == IDOK)
  {
   if (GetCheckState(IDC_TEXT_ITEM_RID) == true)
     m_Type = ProseTextItem::TextItemType::ResourceID;
   else
     m_Type = ProseTextItem::TextItemType::ProseID;
   Close(DialogResult::OK);
  }
 if (child == IDCANCEL)
   Close(DialogResult::Cancel);

 return ret;
}