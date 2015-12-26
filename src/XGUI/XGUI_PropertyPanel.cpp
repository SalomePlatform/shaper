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

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

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

//#define DEBUG_TAB_WIDGETS

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
            this,    SLOT(onActivateNextWidget(ModuleBase_ModelWidget*)));
    connect(aWidget, SIGNAL(keyReleased(QKeyEvent*)),
            this,    SIGNAL(keyReleased(QKeyEvent*)));
    connect(aWidget, SIGNAL(enterClicked()),
            this,    SIGNAL(enterClicked()));

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
  // it is possible that the property panel widgets have not been visualized
  // (e.g. on start operation), so it is strictly important to do not check visualized state
  activateNextWidget(theWidget, false);
}

void XGUI_PropertyPanel::onActivateNextWidget(ModuleBase_ModelWidget* theWidget)
{
  // this slot happens when some widget lost focus, the next widget which accepts the focus
  // should be shown, so the second parameter is true
  // it is important for features where in cases the same attributes are used, isCase for this
  // attribute returns true, however it can be placed in hidden stack widget(extrusion: elements,
  // sketch multi rotation -> single/full point)
  // it is important to check the widget visibility to do not check of the next widget visible
  // state if the current is not shown. (example: sketch circle re-entrant operation after mouse
  // release in the viewer, next, radius, widget should be activated but the first is not visualized)
  bool isVisible = theWidget->isVisible();
  activateNextWidget(theWidget, isVisible);
}


void XGUI_PropertyPanel::activateNextWidget(ModuleBase_ModelWidget* theWidget,
                                            const bool isCheckVisibility)
{
  // TO CHECK: Editing operation does not have automatical activation of widgets
  if (isEditingMode()) {
    activateWidget(NULL);
    return;
  }
  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();

  QList<ModuleBase_ModelWidget*>::const_iterator anIt = myWidgets.begin(), aLast = myWidgets.end();
  bool isFoundWidget = false;
  ModuleBase_Tools::activateWindow(this, "XGUI_PropertyPanel::activateNextWidget()");
  for (; anIt != aLast; anIt++) {
    ModuleBase_ModelWidget* aCurrentWidget = *anIt;
    if (isFoundWidget || !theWidget) {

      if (!aValidators->isCase(aCurrentWidget->feature(), aCurrentWidget->attributeID()))
        continue; // this attribute is not participated in the current case
      if (isCheckVisibility && !aCurrentWidget->isInternal()) {
        if (!aCurrentWidget->isVisible())
          continue;
      }
      if (!aCurrentWidget->isObligatory())
        continue; // not obligatory widgets are not activated automatically

      if (aCurrentWidget->focusTo()) {
        return;
      }
    }
    isFoundWidget = isFoundWidget || (*anIt) == theWidget;
  }
  // set focus to Ok/Cancel button in Property panel if there are no more active widgets
  // it should be performed before activateWidget(NULL) because it emits some signals which
  // can be processed by moudule for example as to activate another widget with setting focus
  QWidget* aNewFocusWidget = 0;
  QToolButton* anOkBtn = findChild<QToolButton*>(PROP_PANEL_OK);
  if (anOkBtn->isEnabled())
    aNewFocusWidget = anOkBtn;
  else {
    QToolButton* aCancelBtn = findChild<QToolButton*>(PROP_PANEL_CANCEL);
    if (aCancelBtn->isEnabled())
      aNewFocusWidget = aCancelBtn;
  }
  if (aNewFocusWidget)
    aNewFocusWidget->setFocus(Qt::TabFocusReason);

  activateWidget(NULL);
}

void findDirectChildren(QWidget* theParent, QList<QWidget*>& theWidgets, const bool theDebug)
{
  QList<QWidget*> aWidgets;

  if (theParent) {
    QLayout* aLayout = theParent->layout();
    if (aLayout) {
      for (int i = 0, aCount = aLayout->count(); i < aCount; i++) {
        QLayoutItem* anItem = aLayout->itemAt(i);
        QWidget* aWidget = anItem ? anItem->widget() : 0;
        if (aWidget) {
          if (aWidget->isVisible()) {
            if (aWidget->focusPolicy() != Qt::NoFocus)
              theWidgets.append(aWidget);
            findDirectChildren(aWidget, theWidgets, false);
          }
        }
        else if (anItem->layout()) {
          QLayout* aLayout = anItem->layout();
          for (int i = 0, aCount = aLayout->count(); i < aCount; i++) {
            QLayoutItem* anItem = aLayout->itemAt(i);
            QWidget* aWidget = anItem ? anItem->widget() : 0;
            if (aWidget) {
              if (aWidget->isVisible()) {
                if (aWidget->focusPolicy() != Qt::NoFocus)
                  theWidgets.append(aWidget);
                findDirectChildren(aWidget, theWidgets, false);
              }
            }
            else {
              // TODO: improve recursive search for the case when here QLayout is used
              // currently, the switch widget uses only 1 level of qlayout in qlayout using for
              // example for construction plane feature. If there are more levels,
              // it should be implemented here
            }
          }
        }
      }
    }
  }
#ifdef DEBUG_TAB_WIDGETS
  if (theDebug) {
    QStringList aWidgetTypes;
    QList<QWidget*>::const_iterator anIt =  theWidgets.begin(), aLast = theWidgets.end();
    for (; anIt != aLast; anIt++) {
      QWidget* aWidget = *anIt;
      if (aWidget)
        aWidgetTypes.append(aWidget->objectName());
    }
    QString anInfo = QString("theWidgets[%1]: %2").arg(theWidgets.count()).arg(aWidgetTypes.join(","));
    qDebug(anInfo.toStdString().c_str());
  }
#endif
}

bool XGUI_PropertyPanel::focusNextPrevChild(bool theIsNext)
{
  // it wraps the Tabs clicking to follow in the chain:
  // controls, last control, Apply, Cancel, first control, controls
  bool isChangedFocus = false;

  QWidget* aFocusWidget = focusWidget();
#ifdef DEBUG_TAB_WIDGETS
  if (aFocusWidget) {
    QString anInfo = QString("focus Widget: %1").arg(aFocusWidget->objectName());
    qDebug(anInfo.toStdString().c_str());
  }
#endif

  QWidget* aNewFocusWidget = 0;
  if (aFocusWidget) {
    QList<QWidget*> aChildren;
    findDirectChildren(this, aChildren, true);
    int aChildrenCount = aChildren.count();
    int aFocusWidgetIndex = aChildren.indexOf(aFocusWidget);
    if (aFocusWidgetIndex >= 0) {
      if (theIsNext) {
        if (aFocusWidgetIndex == aChildrenCount-1) {
          // after the last widget focus should be set to "Apply"
          QToolButton* anOkBtn = findChild<QToolButton*>(PROP_PANEL_OK);
          if (anOkBtn->isEnabled())
            aNewFocusWidget = anOkBtn;
          else {
            QToolButton* aCancelBtn = findChild<QToolButton*>(PROP_PANEL_CANCEL);
            if (aCancelBtn->isEnabled())
              aNewFocusWidget = aCancelBtn;
          }
        }
        else {
          aNewFocusWidget = aChildren[aFocusWidgetIndex+1];
        }
      }
      else {
        if (aFocusWidgetIndex == 0) {
          // before the first widget, the last should accept focus
          aNewFocusWidget = aChildren[aChildrenCount - 1];
        }
        else {
          // before the "Apply" button, the last should accept focus for consistency with "Next"
          QToolButton* anOkBtn = findChild<QToolButton*>(PROP_PANEL_OK);
          if (aFocusWidget == anOkBtn) {
            aNewFocusWidget = aChildren[aChildrenCount - 1];
          }
          else {
            aNewFocusWidget = aChildren[aFocusWidgetIndex-1];
          }
        }
      }
    }
  }
  if (aNewFocusWidget) {
    if (myActiveWidget) {
      myActiveWidget->getControls();
      bool isFirstControl = !theIsNext;
      QWidget* aLastFocusControl = myActiveWidget->getControlAcceptingFocus(isFirstControl);
      if (aFocusWidget == aLastFocusControl) {
        setActiveWidget(NULL);
      }
    }

    //ModuleBase_Tools::setFocus(aNewFocusWidget, "XGUI_PropertyPanel::focusNextPrevChild()");
    aNewFocusWidget->setFocus(theIsNext ? Qt::TabFocusReason : Qt::BacktabFocusReason);
    isChangedFocus = true;
  }
  return isChangedFocus;
}

void XGUI_PropertyPanel::activateNextWidget()
{
  activateNextWidget(myActiveWidget);
}

void XGUI_PropertyPanel::activateWidget(ModuleBase_ModelWidget* theWidget)
{
  std::string aPreviosAttributeID;
  if(myActiveWidget)
    aPreviosAttributeID = myActiveWidget->attributeID();

  // Avoid activation of already actve widget. It could happen on focusIn event many times
  if (setActiveWidget(theWidget)) {
    emit widgetActivated(myActiveWidget);
    if (!myActiveWidget && !isEditingMode()) {
      emit noMoreWidgets(aPreviosAttributeID);
    }
  }
}

bool XGUI_PropertyPanel::setActiveWidget(ModuleBase_ModelWidget* theWidget)
{
  // Avoid activation of already actve widget. It could happen on focusIn event many times
  if (theWidget == myActiveWidget) {
    return false;
  }
  std::string aPreviosAttributeID;
  if(myActiveWidget) {
    aPreviosAttributeID = myActiveWidget->attributeID();
    myActiveWidget->deactivate();
    myActiveWidget->setHighlighted(false);
  }
  if(theWidget) {
    emit beforeWidgetActivated(theWidget);
    theWidget->setHighlighted(true);
    theWidget->activate();
  }
  myActiveWidget = theWidget;
  return true;
}

void XGUI_PropertyPanel::setFocusOnOkButton()
{
  QToolButton* anOkBtn = findChild<QToolButton*>(PROP_PANEL_OK);
  ModuleBase_Tools::setFocus(anOkBtn, "XGUI_PropertyPanel::setFocusOnOkButton()");
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
      myOperationMgr->abortOperation(aOp);
      if (myOperationMgr->hasOperation())
        theEvent->ignore();
      else
        theEvent->accept();
    } else 
      theEvent->ignore();
  } else
    ModuleBase_IPropertyPanel::closeEvent(theEvent);
}
