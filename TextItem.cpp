#include "pch.h"
#include "app.h"

TextItem::TextItem()
{
 ID.Clear();
}

TextItem::TextItem(String id, TextItemType ttype)
{
 ItemType = ttype;
 ID = id;
}

TextItem::~TextItem()
{
}