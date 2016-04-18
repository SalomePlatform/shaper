// TEMPORARY COMMIT
/*
 * ModuleBase_ActionInfo.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: sbh
 */

#include <ModuleBase_ActionInfo.h>
#include <ModuleBase_IconFactory.h>

ModuleBase_ActionInfo::ModuleBase_ActionInfo()
{
  initDefault();
}

ModuleBase_ActionInfo::ModuleBase_ActionInfo(const QString &theText)
{
  initDefault();
}

ModuleBase_ActionInfo::ModuleBase_ActionInfo(const QIcon & theIcon, const QString &theText)
{
  initDefault();
  icon = theIcon;
  text = theText;
}

ModuleBase_ActionInfo::~ModuleBase_ActionInfo()
{
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
}

void ModuleBase_ActionInfo::initFrom(std::shared_ptr<Config_FeatureMessage> theMessage)
{
  id = QString::fromStdString(theMessage->id());
  iconFile = QString::fromStdString(theMessage->icon());
  if (!iconFile.isEmpty()) {
    icon = ModuleBase_IconFactory::loadIcon(iconFile);
  }
  text = QString::fromStdString(theMessage->text());
  toolTip = QString::fromStdString(theMessage->tooltip());
  QString aShortcutStr = QString::fromStdString(theMessage->keysequence());
  if (!aShortcutStr.isEmpty()) {
    shortcut = QKeySequence(aShortcutStr);
  }
  // If feature requires PropertyPannel for input, it should be checkable
  checkable = theMessage->isUseInput();
}

QAction* ModuleBase_ActionInfo::makeAction(QObject* theParent)
{
  QAction* aResult = new QAction(icon, text, theParent);
  aResult->setCheckable(checkable);
  aResult->setChecked(checked);
  aResult->setEnabled(enabled);
  aResult->setVisible(visible);
  aResult->setIconText(iconText);
  aResult->setToolTip(toolTip);
  // aResult->setStatusTip(statusTip);
  // aResult->setWhatsThis(whatsThis);
  aResult->setShortcut(shortcut);
  aResult->setFont(font);
  // By convenience, QAction for a feature keeps feature's id as data (QVariant);
  aResult->setData(id);
  return aResult;
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
