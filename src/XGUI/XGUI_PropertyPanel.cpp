// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

/*
 * XGUI_PropertyPanel.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: sbh
 */

#include <XGUI_PropertyPanel.h>
#include <XGUI_ActionsMgr.h>
//#include <AppElements_Constants.h>
#include <ModuleBase_WidgetMultiSelector.h>

#include <QEvent>
#include <QFrame>
#include <QIcon>
#include <QKeyEvent>
#include <QLayoutItem>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QToolButton>
#include <QAction>

#ifdef _DEBUG
#include <iostream>
#endif

XGUI_PropertyPanel::XGUI_PropertyPanel(QWidget* theParent)
    : ModuleBase_IPropertyPanel(theParent), 
    myActiveWidget(NULL)
{
  this->setWindowTitle(tr("Property Panel"));
  QAction* aViewAct = this->toggleViewAction();
  this->setObjectName(PROP_PANEL);
  setStyleSheet("::title { position: relative; padding-left: 5px; text-align: left center }");

  QWidget* aContent = new QWidget(this);
  myMainLayout = new QVBoxLayout(aContent);
  myMainLayout->setContentsMargins(3, 3, 3, 3);
  this->setWidget(aContent);

  QFrame* aFrm = new QFrame(aContent);
  aFrm->setFrameStyle(QFrame::Sunken);
  aFrm->setFrameShape(QFrame::Panel);
  QHBoxLayout* aBtnLay = new QHBoxLayout(aFrm);
  aBtnLay->setContentsMargins(0, 0, 0, 0);
  myMainLayout->addWidget(aFrm);

  QStringList aBtnNames;
  aBtnNames << QString(PROP_PANEL_HELP)
            << QString(PROP_PANEL_OK)
            << QString(PROP_PANEL_CANCEL);
  foreach(QString eachBtnName, aBtnNames) {
    QToolButton* aBtn = new QToolButton(aFrm);
    aBtn->setObjectName(eachBtnName);
    aBtn->setAutoRaise(true);
    aBtnLay->addWidget(aBtn);
  }
  aBtnLay->insertStretch(1, 1);
  // aBtn->setShortcut(QKeySequence(Qt::Key_Escape));

  myCustomWidget = new QWidget(aContent);
  myMainLayout->addWidget(myCustomWidget);
  setStretchEnabled(true);
}

XGUI_PropertyPanel::~XGUI_PropertyPanel()
{
}

void XGUI_PropertyPanel::cleanContent()
{
  if (myActiveWidget)
    myActiveWidget->deactivate();
  myWidgets.clear();
  qDeleteAll(myCustomWidget->children());
  myActiveWidget = NULL;
  setWindowTitle(tr("Property Panel"));
}

void XGUI_PropertyPanel::setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets)
{
  myWidgets = theWidgets;
  if (theWidgets.empty()) return;
  bool isEnableStretch = true;
  QList<ModuleBase_ModelWidget*>::const_iterator anIt = theWidgets.begin(), aLast =
      theWidgets.end();
  for (; anIt != aLast; anIt++) {
    connect(*anIt, SIGNAL(keyReleased(QKeyEvent*)), this, SIGNAL(keyReleased(QKeyEvent*)));
    connect(*anIt, SIGNAL(focusOutWidget(ModuleBase_ModelWidget*)),
            this,  SLOT(activateNextWidget(ModuleBase_ModelWidget*)));
    connect(*anIt, SIGNAL(focusInWidget(ModuleBase_ModelWidget*)),
            this,  SLOT(activateWidget(ModuleBase_ModelWidget*)));

    //ModuleBase_WidgetPoint2D* aPointWidget = dynamic_cast<ModuleBase_WidgetPoint2D*>(*anIt);
    //if (aPointWidget)
    //  connect(aPointWidget, SIGNAL(storedPoint2D(ObjectPtr, const std::string&)), this,
    //          SIGNAL(storedPoint2D(ObjectPtr, const std::string&)))
    //}

    if (!isEnableStretch) continue;
    foreach(QWidget* eachWidget, (*anIt)->getControls()) {
      QSizePolicy::Policy aVPolicy = eachWidget->sizePolicy().verticalPolicy();
      if(aVPolicy == QSizePolicy::Expanding ||
         aVPolicy == QSizePolicy::MinimumExpanding) {
        isEnableStretch = false;
      }
    }
  }
  setStretchEnabled(isEnableStretch);
  ModuleBase_ModelWidget* aLastWidget = theWidgets.last();
  if (aLastWidget) {
    QList<QWidget*> aControls = aLastWidget->getControls();
    if (!aControls.empty()) {
      QWidget* aLastControl = aControls.last();

      QToolButton* anOkBtn = findChild<QToolButton*>(PROP_PANEL_OK);
      QToolButton* aCancelBtn = findChild<QToolButton*>(PROP_PANEL_CANCEL);

      setTabOrder(aLastControl, anOkBtn);
      setTabOrder(anOkBtn, aCancelBtn);
    }
  }
}

const QList<ModuleBase_ModelWidget*>& XGUI_PropertyPanel::modelWidgets() const
{
  return myWidgets;
}

QWidget* XGUI_PropertyPanel::contentWidget()
{
  return myCustomWidget;
}

void XGUI_PropertyPanel::updateContentWidget(FeaturePtr theFeature)
{
  // Invalid feature case on abort of the operation
  if (theFeature.get() == NULL)
    return;
  if (theFeature->isAction() || !theFeature->data())
    return;
  foreach(ModuleBase_ModelWidget* eachWidget, myWidgets)
  {
    eachWidget->setFeature(theFeature);
    eachWidget->restoreValue();
  }
  // the repaint is used here to immediately react in GUI to the values change.
  repaint();
}

void XGUI_PropertyPanel::activateNextWidget(ModuleBase_ModelWidget* theWidget)
{
  // TO CHECK: Editing operation does not have automatical activation of widgets
  if (isEditingMode()) {
    activateWidget(NULL);
    return;
  }
  ModuleBase_ModelWidget* aNextWidget = 0;
  QList<ModuleBase_ModelWidget*>::const_iterator anIt = myWidgets.begin(), aLast = myWidgets.end();
  bool isFoundWidget = false;
  for (; anIt != aLast && !aNextWidget; anIt++) {
    if (isFoundWidget || !theWidget) {
      if ((*anIt)->focusTo()) {
        aNextWidget = *anIt;
      }
    }
    isFoundWidget = (*anIt) == theWidget;
  }
  // Normaly focusTo is enough to activate widget
  // here is a special case on mouse click in the viewer
  //if(aNextWidget == NULL) {
    activateWidget(aNextWidget);
  //}
}

void XGUI_PropertyPanel::setStretchEnabled(bool isEnabled)
{
  if (myMainLayout->count() == 0)
    return;
  int aStretchIdx = myMainLayout->count() - 1;
  bool hasStretch = myMainLayout->itemAt(aStretchIdx)->spacerItem() != NULL;
  QLayoutItem* aChild;
  if (isEnabled) {
    if (!hasStretch) myMainLayout->addStretch(1);
  } else if (hasStretch) {
    aChild = myMainLayout->takeAt(aStretchIdx);
    delete aChild;
  }
}

void XGUI_PropertyPanel::activateNextWidget()
{
  activateNextWidget(myActiveWidget);
}

void XGUI_PropertyPanel::activateWidget(ModuleBase_ModelWidget* theWidget)
{
  // Avoid activation of already actve widget. It could happen on focusIn event many times
  if (theWidget == myActiveWidget)
    return;
  if(myActiveWidget) {
    myActiveWidget->deactivate();
    myActiveWidget->setHighlighted(false);
  }
  if(theWidget) {
    if (theWidget)
      emit beforeWidgetActivated(theWidget);
    theWidget->activate();
    theWidget->setHighlighted(true);
  }
  myActiveWidget = theWidget;
  if (myActiveWidget)
    emit widgetActivated(theWidget);
  else if (!isEditingMode())
    emit noMoreWidgets();
}

void XGUI_PropertyPanel::setCancelEnabled(bool theEnabled)
{
  QToolButton* anCancelBtn = findChild<QToolButton*>(PROP_PANEL_CANCEL);
  anCancelBtn->setEnabled(theEnabled);
}

bool XGUI_PropertyPanel::isCancelEnabled() const
{
  QToolButton* anCancelBtn = findChild<QToolButton*>(PROP_PANEL_CANCEL);
  return anCancelBtn->isEnabled();
}

void XGUI_PropertyPanel::setEditingMode(bool isEditing)
{
  ModuleBase_IPropertyPanel::setEditingMode(isEditing);
  foreach(ModuleBase_ModelWidget* aWgt, myWidgets) {
    aWgt->setEditingMode(isEditing);
  }
}

void XGUI_PropertyPanel::setupActions(XGUI_ActionsMgr* theMgr)
{
  QStringList aButtonNames;
  aButtonNames << PROP_PANEL_OK << PROP_PANEL_CANCEL << PROP_PANEL_HELP;
  QList<XGUI_ActionsMgr::OperationStateActionId> aActionIds;
  aActionIds << XGUI_ActionsMgr::Accept << XGUI_ActionsMgr::Abort << XGUI_ActionsMgr::Help;
  for (int i = 0; i < aButtonNames.size(); ++i) {
    QToolButton* aBtn = findChild<QToolButton*>(aButtonNames.at(i));
    QAction* anAct = theMgr->operationStateAction(aActionIds.at(i));
    aBtn->setDefaultAction(anAct);
  }
}
