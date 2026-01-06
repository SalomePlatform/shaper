// Copyright (C) 2014-2026  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <ModuleBase_ActionInfo.h>
#include <ModuleBase_IconFactory.h>
#include <ModuleBase_Tools.h>


// #define ModuleBase_ActionInfo_DBG
#ifdef ModuleBase_ActionInfo_DBG
#include <iostream>
#endif //ModuleBase_ActionInfo_DBG

ModuleBase_ActionInfo::ModuleBase_ActionInfo()
{
  initDefault();
}

ModuleBase_ActionInfo::ModuleBase_ActionInfo(const QString &theText)
{
  initDefault();
  text = theText;
}

ModuleBase_ActionInfo::ModuleBase_ActionInfo(const QIcon & theIcon, const QString &theText)
{
  initDefault();
  icon = theIcon;
  text = theText;
}

void ModuleBase_ActionInfo::initFrom(QAction* theAction)
{
  // By convenience, QAction for a feature keeps feature's id as data (QVariant);
  if (theAction->data().isValid()) {
    id = theAction->data().toString();
  }
  checkable = theAction->isCheckable();
  checked = theAction->isChecked();
  enabled = theAction->isEnabled();
  visible = theAction->isVisible();
  icon = theAction->icon();
  text = theAction->text();
  iconText = theAction->iconText();
  toolTip = theAction->toolTip();
  // statusTip = theAction->statusTip();
  // whatsThis = theAction->whatsThis();
  shortcut = theAction->shortcut();
  font = theAction->font();

  #ifdef ModuleBase_ActionInfo_DBG
    std::wcout << "ModuleBase_ActionInfo::initFrom(QAction*). State after: " << toString().toStdWString() << std::endl;
  #endif
}

void ModuleBase_ActionInfo::initFrom(std::shared_ptr<Config_FeatureMessage> theMessage)
{
  id = QString::fromStdString(theMessage->id());
  workbenchID = QString::fromStdString(theMessage->workbenchId());

  iconFile = QString::fromStdString(theMessage->icon());
  if (!iconFile.isEmpty()) {
    icon = ModuleBase_IconFactory::loadIcon(iconFile);
  }
  text = QString::fromUtf8(theMessage->text().c_str());
  toolTip = QString::fromUtf8(theMessage->tooltip().c_str());
  toolBar = QString::fromStdString(theMessage->toolBarId());
  QString aShortcutStr = QString::fromStdString(theMessage->keysequence());
  if (!aShortcutStr.isEmpty()) {
    shortcut = QKeySequence(aShortcutStr);
  }
  // If feature requires PropertyPannel for input, it should be checkable
  checkable = theMessage->isUseInput();
  // If Feature requires modal Dialog box for input
  modal = theMessage->isModal();

  #ifdef ModuleBase_ActionInfo_DBG
    std::wcout << "ModuleBase_ActionInfo::initFrom(std::shared_ptr<Config_FeatureMessage>). State after: " << toString().toStdWString() << std::endl;
  #endif
}

void ModuleBase_ActionInfo::initDefault()
{
  id = QString();
  checkable = false;
  checked   = false;
  enabled   = true;
  visible   = true;
  icon = QIcon();
  text = QString();
  iconText = QString();
  iconFile = QString();
  toolTip = QString();
  // statusTip = QString();
  // whatsThis = QString();
  shortcut = QKeySequence();
  font = QFont();
}

QString ModuleBase_ActionInfo::toString() const
{
  QString res = "ModuleBase_ActionInfo {\n";
  res += "\tid: \"" + id + "\";\n";

  if (!workbenchID.isEmpty())
    res += "\tworkbenchID: \"" + workbenchID + "\";\n";

  res += QString("\tcheckable: ") + (checkable ? "+" : "-") +
  "; checked: " + (checked ? "+" : "-") +
  "; enabled: " + (enabled ? "+" : "-") +
  "; visible: " + (visible ? "+" : "-") +
  "; modal: " + (modal ? "+" : "-") + ";\n";

  if (!text.isEmpty())
    res += "\ttext: \"" + text + "\";\n";

  if (!iconText.isEmpty())
    res += "\ticonText: \"" + iconText + "\";\n";

  if (!iconFile.isEmpty())
    res += "\ticonFile: \"" + iconFile + "\";\n";

  if (!toolTip.isEmpty())
    res += "\ttoolTip: \"" + toolTip + "\";\n";

  if (!toolBar.isEmpty())
    res += "\ttoolBar: \"" + toolBar + "\";\n";

  if (!shortcut.isEmpty())
    res += "\tshortcut: \"" + shortcut.toString() + "\";\n";

  res += "};\n";
  return res;
}
