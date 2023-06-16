// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include <XGUI_ActionsMgr.h>
#include <XGUI_ActiveControlMgr.h>
#include <XGUI_SelectionActivate.h>
#include <XGUI_ActiveControlSelector.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_PropertyPanelSelector.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>

//#include <AppElements_Constants.h>
#include <ModuleBase_WidgetMultiSelector.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_PageWidget.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_Events.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_IWorkshop.h>

#include <Events_Loop.h>

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
#include <QAction>
#include <QScrollArea>

#ifdef _DEBUG
#include <iostream>
#endif

//#define DEBUG_TAB_WIDGETS

//#define DEBUG_ACTIVE_WIDGET

XGUI_PropertyPanel::XGUI_PropertyPanel(QWidget* theParent, XGUI_OperationMgr* theMgr)
    : ModuleBase_IPropertyPanel(theParent),
    myPanelPage(NULL),
    myActiveWidget(NULL),
    myPreselectionWidget(NULL),
    myInternalActiveWidget(NULL),
    myOperationMgr(theMgr)
{
  setWindowTitle(tr("Property Panel"));
  //MAYBE_UNUSED QAction* aViewAct = toggleViewAction();
  setObjectName(PROP_PANEL);
  setStyleSheet("::title { position: relative; padding-left: 5px; text-align: left center }");

  QWidget* aContent = new QWidget(this);
  QGridLayout* aMainLayout = new QGridLayout(aContent);
  const int kPanelColumn = 0;
  int aPanelRow = 0;
  aMainLayout->setContentsMargins(3, 3, 3, 3);
  setWidget(aContent);

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
    << QString(PROP_PANEL_OK_PLUS)
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

  // spit to make the preview button on the bottom of the panel
  aMainLayout->setRowStretch(aPanelRow++, 1);

  // preview button on the bottom of panel
  aFrm = new QFrame(aContent);
  aBtnLay = new QHBoxLayout(aFrm);
  aBtnLay->addStretch(1);
  ModuleBase_Tools::zeroMargins(aBtnLay);
  aMainLayout->addWidget(aFrm, aPanelRow++, kPanelColumn);

  QToolButton* aBtn = new QToolButton(aFrm);
  aBtn->setObjectName(PROP_PANEL_PREVIEW);
  aBtnLay->addWidget(aBtn);
}

XGUI_PropertyPanel::~XGUI_PropertyPanel()
{
}

void XGUI_PropertyPanel::cleanContent()
{
  if (myActiveWidget)
    myActiveWidget->deactivate();

  XGUI_ActiveControlSelector* aPPSelector = XGUI_Tools::workshop(myOperationMgr->workshop())->
    activeControlMgr()->getSelector(XGUI_PropertyPanelSelector::Type());
  aPPSelector->reset(); // it removes need to be updated widget link

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
  emit propertyPanelDeactivated();
  // VSV: It seems that this code is not necessary:
  //      it is called on propertyPanelDeactivated() event
  //myOperationMgr->workshop()->selectionActivate()->updateSelectionModes();
  //myOperationMgr->workshop()->selectionActivate()->updateSelectionFilters();
#ifdef DEBUG_ACTIVE_WIDGET
  std::cout << "myActiveWidget = NULL" << std::endl;
#endif

  findButton(PROP_PANEL_PREVIEW)->setVisible(false); /// by default it is hidden
  setWindowTitle(tr("Property Panel"));
}

void XGUI_PropertyPanel::setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets)
{
  myWidgets = theWidgets;
  if (theWidgets.empty())
    return;
  foreach (ModuleBase_ModelWidget* aWidget, theWidgets) {
    connect(aWidget, SIGNAL(focusInWidget(ModuleBase_ModelWidget*)),
            this,    SLOT(onFocusInWidget(ModuleBase_ModelWidget*)));
    connect(aWidget, SIGNAL(focusOutWidget(ModuleBase_ModelWidget*)),
            this,    SLOT(onActivateNextWidget(ModuleBase_ModelWidget*)));
    connect(aWidget, SIGNAL(keyReleased(QObject*, QKeyEvent*)),
            this,    SIGNAL(keyReleased(QObject*, QKeyEvent*)));
    connect(aWidget, SIGNAL(enterClicked(QObject*)),
            this,    SIGNAL(enterClicked(QObject*)));
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
  update();
}

void XGUI_PropertyPanel::createContentPanel(FeaturePtr theFeature)
{
  // Invalid feature case on abort of the operation
  if (theFeature.get() == NULL)
    return;
  if (theFeature->isAction() || !theFeature->data())
    return;

  ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
  if (myWidgets.empty()) {
    QString aXmlRepr = anOperation->getDescription()->xmlRepresentation();

    ModuleBase_WidgetFactory aFactory(aXmlRepr.toStdString(), myOperationMgr->workshop());
    ModuleBase_PageBase* aPage = contentWidget();
    aFactory.createPanel(aPage, theFeature);
    // update model widgets if exist
    setModelWidgets(aPage->modelWidgets());
    // Apply button should be update if the feature was modified by the panel
    myOperationMgr->onValidateOperation();
  }
  ModuleBase_OperationFeature* aFeatureOp =
    dynamic_cast<ModuleBase_OperationFeature*>(anOperation);
  if (aFeatureOp && (!aFeatureOp->isEditOperation()))
    updateApplyPlusButton(theFeature);
  else
    findButton(PROP_PANEL_OK_PLUS)->setVisible(false);
}

void XGUI_PropertyPanel::updateApplyPlusButton(FeaturePtr theFeature)
{
  if (theFeature.get()) {
    std::shared_ptr<Config_FeatureMessage> aFeatureInfo =
      myOperationMgr->workshop()->featureInfo(theFeature->getKind().c_str());
    if (aFeatureInfo.get()) {
      findButton(PROP_PANEL_OK_PLUS)->setVisible(aFeatureInfo->isApplyContinue());
      return;
    }
  }
  findButton(PROP_PANEL_OK_PLUS)->setVisible(false);
}

ModuleBase_ModelWidget* XGUI_PropertyPanel::activeWidget(const bool isUseCustomWidget) const
{
  if (isUseCustomWidget && myInternalActiveWidget)
    return myInternalActiveWidget;

  return myActiveWidget;
}

void XGUI_PropertyPanel::activateNextWidget(ModuleBase_ModelWidget* theWidget)
{
  // it is possible that the property panel widgets have not been visualized
  // (e.g. on start operation), so it is strictly important to do not check visualized state
  activateNextWidget(theWidget, false);
}

void XGUI_PropertyPanel::onFocusInWidget(ModuleBase_ModelWidget* theWidget)
{
#ifdef DEBUG_ACTIVE_WIDGET
  std::cout << "onFocusInWidget" << std::endl;
#endif
  if (theWidget->canAcceptFocus())
    activateWidget(theWidget);
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
  // release in the viewer, next, radius,
  // widget should be activated but the first is not visualized)
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
        continue; // this attribute does not participate in the current case
      if (isCheckVisibility && !aCurrentWidget->isInternal()) {
        if (!aCurrentWidget->isVisible())
          continue;
      }
      if (!aCurrentWidget->isObligatory())
        continue; // not obligatory widgets are not activated automatically

      if (!aCurrentWidget->canAcceptFocus())
        continue; // do not set focus if it can not be accepted, case: optional choice

      if (aCurrentWidget->focusTo()) {
        aCurrentWidget->emitFocusInWidget();
        return;
      }
    }
    isFoundWidget = isFoundWidget || (*anIt) == theWidget;
  }
  // set focus to Ok/Cancel button in Property panel if there are no more active widgets
  // it should be performed before activateWidget(NULL) because it emits some signals which
  // can be processed by moudule for example as to activate another widget with setting focus
  QWidget* aNewFocusWidget = 0;
  QToolButton* anOkBtn = findButton(PROP_PANEL_OK);
  if (anOkBtn->isEnabled())
    aNewFocusWidget = anOkBtn;
  else {
    QToolButton* aCancelBtn = findButton(PROP_PANEL_CANCEL);
    if (aCancelBtn->isEnabled())
      aNewFocusWidget = aCancelBtn;
  }
  if (aNewFocusWidget)
    ModuleBase_Tools::setFocus(aNewFocusWidget, "XGUI_PropertyPanel::activateNextWidget");

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
          QLayout* anItemLayout = anItem->layout();
          for (int it = 0, cnt = anItemLayout->count(); it < cnt; it++) {
            QLayoutItem* aCurItem = anItemLayout->itemAt(it);
            QWidget* aCurWidget = aCurItem ? aCurItem->widget() : 0;
            if (aCurWidget) {
              if (aCurWidget->isVisible()) {
                if (aCurWidget->focusPolicy() != Qt::NoFocus)
                  theWidgets.append(aCurWidget);
                findDirectChildren(aCurWidget, theWidgets, false);
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
    QString anInfo = QString("theWidgets[%1]: %2")
      .arg(theWidgets.count()).arg(aWidgetTypes.join(","));
    qDebug(anInfo.toStdString().c_str());
  }
#endif
}

bool XGUI_PropertyPanel::setFocusNextPrevChild(bool theIsNext)
{
  return focusNextPrevChild(theIsNext);
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
  ModuleBase_ModelWidget* aFocusMWidget = ModuleBase_ModelWidget::findModelWidget(this,
                                                                         aFocusWidget);
  //if (aFocusMWidget)
  //  aFocusMWidget->setHighlighted(false);

  QWidget* aNewFocusWidget = 0;
  if (aFocusWidget) {
    QList<QWidget*> aChildren;
    findDirectChildren(this, aChildren, true);
    int aChildrenCount = aChildren.count();
    int aFocusWidgetIndex = aChildren.indexOf(aFocusWidget);
    QToolButton* anOkBtn = findButton(PROP_PANEL_OK);
    if (aFocusWidgetIndex >= 0) {
      if (theIsNext) {
        if (aFocusWidgetIndex == aChildrenCount-1) {
          // after the last widget focus should be set to "Apply"
          if (anOkBtn->isEnabled())
            aNewFocusWidget = anOkBtn;
          else {
            QToolButton* aCancelBtn = findButton(PROP_PANEL_CANCEL);
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
      bool isFirstControl = !theIsNext;
      QWidget* aLastFocusControl = myActiveWidget->getControlAcceptingFocus(isFirstControl);
      if (aFocusWidget == aLastFocusControl) {
        setActiveWidget(NULL, false);
      }
    }

    // we want to have property panel as an active window to enter values in double control
    ModuleBase_Tools::setFocus(aNewFocusWidget, "XGUI_PropertyPanel::focusNextPrevChild()");

    ModuleBase_ModelWidget* aNewFocusMWidget = ModuleBase_ModelWidget::findModelWidget(this,
                                                                              aNewFocusWidget);
    if (aNewFocusMWidget) {
      if (aFocusMWidget && (aFocusMWidget != aNewFocusMWidget)) {
        aFocusMWidget->setHighlighted(false);
      }
      aNewFocusMWidget->emitFocusInWidget();
      isChangedFocus = true;
    }
  }
  return isChangedFocus;
}

void XGUI_PropertyPanel::activateNextWidget()
{
  activateNextWidget(myActiveWidget);
}

void XGUI_PropertyPanel::activateWidget(ModuleBase_ModelWidget* theWidget, const bool theEmitSignal)
{
  std::string aPreviosAttributeID;
  if(myActiveWidget)
    aPreviosAttributeID = myActiveWidget->attributeID();

  // Avoid activation of already actve widget. It could happen on focusIn event many times
  setActiveWidget(theWidget, theEmitSignal);
}

bool XGUI_PropertyPanel::setActiveWidget(ModuleBase_ModelWidget* theWidget, const bool isEmitSignal)
{
  // Avoid activation of already actve widget. It could happen on focusIn event many times
  if (theWidget == myActiveWidget) {
    return false;
  }
  std::string aPreviosAttributeID;
  ModuleBase_ModelWidget* aDeactivatedWidget = NULL, *anActivatedWidget = NULL;
  if(myActiveWidget) {
    aPreviosAttributeID = myActiveWidget->attributeID();
    myActiveWidget->processValueState();
    myActiveWidget->deactivate();
    myActiveWidget->setHighlighted(false);
    aDeactivatedWidget = myActiveWidget;
  }
  if(theWidget) {
    emit beforeWidgetActivated(theWidget);
    theWidget->setHighlighted(true);
    theWidget->activate();
    anActivatedWidget = theWidget;
  }
  myActiveWidget = theWidget;

#ifdef DEBUG_ACTIVE_WIDGET
  std::cout << "myActiveWidget = " << (theWidget ? theWidget->context().c_str() : "") << std::endl;
#endif
  bool aHasMoreWidgets = true;
  if (isEmitSignal) {
    //emit widgetActivated(myActiveWidget);
    if (!myActiveWidget && !isEditingMode()) {
      aHasMoreWidgets = false;
      emit noMoreWidgets(aPreviosAttributeID);
    }
  }
  if (myActiveWidget)
    emit propertyPanelActivated();
  // VSV: Do not deactivate whole property panel on non-active widget
  // because it makes problem for continuing of projection operation
  // because of redefinition of module selection filters while projection
  // operation is not finished.
  //else
  //  emit propertyPanelDeactivated();
  myOperationMgr->workshop()->selectionActivate()->updateSelectionModes();
  myOperationMgr->workshop()->selectionActivate()->updateSelectionFilters();

  if (aHasMoreWidgets && aDeactivatedWidget)
    aDeactivatedWidget->updateAfterDeactivation();
  if (aHasMoreWidgets && anActivatedWidget)
    anActivatedWidget->updateAfterActivation();

  if (aHasMoreWidgets && myActiveWidget)
  {
    // restore widget selection should be done after selection modes of widget activating
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_UPDATE_BY_WIDGET_SELECTION);
    Events_Loop::loop()->flush(anEvent);
  }
  return true;
}

void XGUI_PropertyPanel::setFocusOnOkButton()
{
  QToolButton* anOkBtn = findButton(PROP_PANEL_OK);
  ModuleBase_Tools::setFocus(anOkBtn, "XGUI_PropertyPanel::setFocusOnOkButton()");
}

void XGUI_PropertyPanel::setCancelEnabled(bool theEnabled)
{
  QToolButton* anCancelBtn = findButton(PROP_PANEL_CANCEL);
  anCancelBtn->setEnabled(theEnabled);
}

bool XGUI_PropertyPanel::isCancelEnabled() const
{
  QToolButton* anCancelBtn = findButton(PROP_PANEL_CANCEL);
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
  aButtonNames << PROP_PANEL_OK<< PROP_PANEL_OK_PLUS << PROP_PANEL_CANCEL
               << PROP_PANEL_HELP << PROP_PANEL_PREVIEW;
  QList<XGUI_ActionsMgr::OperationStateActionId> aActionIds;
  aActionIds << XGUI_ActionsMgr::Accept << XGUI_ActionsMgr::AcceptPlus << XGUI_ActionsMgr::Abort
             << XGUI_ActionsMgr::Help << XGUI_ActionsMgr::Preview;
  for (int i = 0; i < aButtonNames.size(); ++i) {
    QToolButton* aBtn = findButton(aButtonNames.at(i).toStdString().c_str());
    QAction* anAct = theMgr->operationStateAction(aActionIds.at(i));
    aBtn->setDefaultAction(anAct);
  }
}

void XGUI_PropertyPanel::onAcceptData()
{
  foreach (ModuleBase_ModelWidget* aWidget, myWidgets) {
    aWidget->onFeatureAccepted();
  }
}

void XGUI_PropertyPanel::setInternalActiveWidget(ModuleBase_ModelWidget* theWidget)
{
  if (theWidget)
  {
    myInternalActiveWidget = theWidget;
    emit propertyPanelActivated();
  }
  else
  {
    if (myInternalActiveWidget)
    {
      delete myInternalActiveWidget;
      myInternalActiveWidget = 0;
    }
    emit propertyPanelDeactivated();
  }
  myOperationMgr->workshop()->selectionActivate()->updateSelectionModes();
  myOperationMgr->workshop()->selectionActivate()->updateSelectionFilters();
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

QToolButton* XGUI_PropertyPanel::findButton(const char* theInternalName) const
{
  return findChild<QToolButton*>(theInternalName);
}
