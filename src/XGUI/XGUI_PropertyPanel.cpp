// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

/*
 * XGUI_PropertyPanel.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: sbh
 */

#include <XGUI_PropertyPanel.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_OperationMgr.h>
//#include <AppElements_Constants.h>
#include <ModuleBase_WidgetMultiSelector.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_PageWidget.h>

#include <QEvent>
#include <QFrame>
#include <QIcon>
#include <QKeyEvent>
#include <QLayoutItem>
#include <QToolButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QToolButton>
#include <QAction>

#ifdef _DEBUG
#include <iostream>
#endif

XGUI_PropertyPanel::XGUI_PropertyPanel(QWidget* theParent, XGUI_OperationMgr* theMgr)
    : ModuleBase_IPropertyPanel(theParent), 
    myActiveWidget(NULL),
    myPreselectionWidget(NULL),
    myPanelPage(NULL),
    myOperationMgr(theMgr)
{
  this->setWindowTitle(tr("Property Panel"));
  QAction* aViewAct = this->toggleViewAction();
  this->setObjectName(PROP_PANEL);
  setStyleSheet("::title { position: relative; padding-left: 5px; text-align: left center }");

  QWidget* aContent = new QWidget(this);
  QGridLayout* aMainLayout = new QGridLayout(aContent);
  const int kPanelColumn = 0;
  int aPanelRow = 0;
  aMainLayout->setContentsMargins(3, 3, 3, 3);
  this->setWidget(aContent);

  QFrame* aFrm = new QFrame(aContent);
  aFrm->setFrameStyle(QFrame::Raised);
  aFrm->setFrameShape(QFrame::Panel);
  QHBoxLayout* aBtnLay = new QHBoxLayout(aFrm);
  ModuleBase_Tools::zeroMargins(aBtnLay);
  aMainLayout->addWidget(aFrm, aPanelRow++, kPanelColumn);

  myHeaderWidget = aFrm;

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

  myPanelPage = new ModuleBase_PageWidget(aContent);
  myPanelPage->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
  aMainLayout->addWidget(myPanelPage, aPanelRow, kPanelColumn);
}

XGUI_PropertyPanel::~XGUI_PropertyPanel()
{
}

void XGUI_PropertyPanel::cleanContent()
{
  if (myActiveWidget)
    myActiveWidget->deactivate();

  /// as the widgets are deleted later, it is important that the signals
  /// of these widgets are not processed. An example of the error is issue 986.
  /// In the given case, the property panel is firstly filled by new widgets
  /// of restarted operation and after that the mouse release signal come from
  /// the widget of the previous operation (Point2d widget about mouse is released
  /// and focus is out of this widget)
  QList<ModuleBase_ModelWidget*>::const_iterator anIt = myWidgets.begin(),
                                                 aLast = myWidgets.end();
  for (; anIt != aLast; anIt++) {
    QWidget* aWidget = *anIt;
    if (aWidget) {
      aWidget->blockSignals(true);
    }
  }

  myWidgets.clear();
  myPanelPage->clearPage();
  myActiveWidget = NULL;
  setWindowTitle(tr("Property Panel"));
}

void XGUI_PropertyPanel::setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets)
{
  myWidgets = theWidgets;
  if (theWidgets.empty()) return;
  foreach (ModuleBase_ModelWidget* aWidget, theWidgets) {
    connect(aWidget, SIGNAL(focusInWidget(ModuleBase_ModelWidget*)),
            this,    SLOT(activateWidget(ModuleBase_ModelWidget*)));
    connect(aWidget, SIGNAL(focusOutWidget(ModuleBase_ModelWidget*)),
            this,    SLOT(activateNextWidget(ModuleBase_ModelWidget*)));
    connect(aWidget, SIGNAL(keyReleased(QKeyEvent*)),
            this,    SIGNAL(keyReleased(QKeyEvent*)));
  }
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

ModuleBase_PageBase* XGUI_PropertyPanel::contentWidget()
{

  return static_cast<ModuleBase_PageBase*>(myPanelPage);
}

void XGUI_PropertyPanel::updateContentWidget(FeaturePtr theFeature)
{
  // Invalid feature case on abort of the operation
  if (theFeature.get() == NULL)
    return;
  if (theFeature->isAction() || !theFeature->data())
    return;
  foreach(ModuleBase_ModelWidget* eachWidget, myWidgets) {
    if (!eachWidget->feature().get())
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
  QList<ModuleBase_ModelWidget*>::const_iterator anIt = myWidgets.begin(), aLast = myWidgets.end();
  bool isFoundWidget = false;
  activateWindow();
  for (; anIt != aLast; anIt++) {
    if (isFoundWidget || !theWidget) {
      if ((*anIt)->focusTo()) {
        return;
      }
    }
    isFoundWidget = (*anIt) == theWidget;
  }
  activateWidget(NULL);
}

void XGUI_PropertyPanel::activateNextWidget()
{
  activateNextWidget(myActiveWidget);
}

void XGUI_PropertyPanel::activateWidget(ModuleBase_ModelWidget* theWidget)
{
  // Avoid activation of already actve widget. It could happen on focusIn event many times
  if (theWidget == myActiveWidget) {
    return;
  }
  if(myActiveWidget) {
    myActiveWidget->deactivate();
    myActiveWidget->setHighlighted(false);
  }
  if(theWidget) {
    emit beforeWidgetActivated(theWidget);
    theWidget->setHighlighted(true);
    theWidget->activate();
  }
  myActiveWidget = theWidget;
  if (myActiveWidget) {
    emit widgetActivated(theWidget);
  } else if (!isEditingMode()) {
    emit noMoreWidgets();
  }
}

void XGUI_PropertyPanel::setFocusOnOkButton()
{
  QToolButton* anOkBtn = findChild<QToolButton*>(PROP_PANEL_OK);
  anOkBtn->setFocus();
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

ModuleBase_ModelWidget* XGUI_PropertyPanel::preselectionWidget() const
{
  return myPreselectionWidget;
}

void XGUI_PropertyPanel::setPreselectionWidget(ModuleBase_ModelWidget* theWidget)
{
  myPreselectionWidget = theWidget;
}


void XGUI_PropertyPanel::closeEvent(QCloseEvent* theEvent)
{
  ModuleBase_Operation* aOp = myOperationMgr->currentOperation();
  if (aOp) {
    if (myOperationMgr->canStopOperation(aOp)) {
      myOperationMgr->abortAllOperations();
      theEvent->accept();
    } else 
      theEvent->ignore();
  } else
    ModuleBase_IPropertyPanel::closeEvent(theEvent);
}