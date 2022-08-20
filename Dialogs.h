#pragma once

class TextItemDlg : public ADialog
{
 public:

 DialogResult Show(AWnd *parent, ProseLanguage const &lang, ProseTextItem const &ti);
 DialogResult Show(AWnd *parent, ProseLanguage const &lang, ProseTextItem::TextItemType iType);

 String GetKey() { return m_Key; }
 String GetTextEntry()  { return m_Text; }
 ProseTextItem::TextItemType GetType() { return m_ItemType; }

 protected:
 virtual void OnInitDialog();
 virtual WMR OnCommand(int id, HWND hChild);

 ProseLanguage m_Language;
 ProseTextItem m_Item;

 String m_Key;
 String m_Text;
 

 ProseTextItem::TextItemType m_ItemType;

};

class DualItemDlg : public ADialog
{
 public:

 DialogResult Show(AWnd *parent, ProseLanguage const &langKnown, ProseLanguage const &targetLang, ProseTextItem const &ti);
 
 String GetIDName() { return m_Key; }
 String GetTextEntry()  { return m_Text; }
 ProseTextItem::TextItemType GetType() { return m_ItemType; }

 protected:
 virtual void OnInitDialog();
 virtual WMR OnCommand(int id, HWND hChild);

 ProseLanguage m_LanguageKnown;
 ProseLanguage m_LanguageTarget;
 ProseTextItem m_Item;

 String m_Key;
 String m_Text;

 EditWnd m_Known;
 EditWnd m_Target;

 ProseTextItem::TextItemType m_ItemType;

};

class SetLanguageDlg : public ADialog
{
 public:

 DialogResult Show(AWnd *parent);

 ProseLanguage KnownLanguage;
 ProseLanguage TargetLanguage;

 protected:

 void OnInitDialog();
 WMR  OnCommand(int child, HWND hChild);

 ADropList m_Known;
 ADropList m_Target;
};

class LanguageDlg : public ADialog
{
 public:

 DialogResult Show(AWnd *parent);
 DialogResult Show(AWnd *parent, ProseLanguage const &lang);

 String LanguageName() { return m_LanguageName; }
 ProseLanguage::LanguageOrientation Orientation() { return m_Orient; }
 
 protected:

 virtual void OnInitDialog();
 virtual WMR OnCommand(int child, HWND hChild);

 int    m_ID;
 String m_LanguageName;

 ProseLanguage::LanguageOrientation m_Orient;

 
};

class LanguageChooseDlg : public ADialog
{
 public:
 enum class ChooseType : int { Drop, Change, Select };

 DialogResult Show(AWnd *parent, ChooseType cType);
 
 ProseLanguage Language() { return m_Lang; }
 
 protected:

 virtual void OnInitDialog();
 virtual WMR OnCommand(int child, HWND hChild);

 ChooseType    m_Type;
 ProseLanguage m_Lang;
 ListView      m_List;

};

class TextTypeDlg : public ADialog
{
 public:

 DialogResult Show(AWnd *parent);

 ProseTextItem::TextItemType TextType() { return m_Type; }

 protected:

 virtual WMR OnCommand(int child, HWND hChild);

 ProseTextItem::TextItemType m_Type;
 
};