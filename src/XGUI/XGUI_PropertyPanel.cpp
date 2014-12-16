// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

/*
 * XGUI_PropertyPanel.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: sbh
 */

#include <XGUI_PropertyPanel.h>
//#include <AppElements_Constants.h>
#include <ModuleBase_WidgetMultiSelector.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QLayoutItem>

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

  QPushButton* aBtn = new QPushButton(QIcon(":pictures/button_help.png"), "", aFrm);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);
  aBtnLay->addStretch(1);
  aBtn = new QPushButton(QIcon(":pictures/button_ok.png"), "", aFrm);
  aBtn->setObjectName(PROP_PANEL_OK);
  aBtn->setToolTip(tr("Ok"));
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);

  aBtn = new QPushButton(QIcon(":pictures/button_cancel.png"), "", aFrm);
  aBtn->setToolTip(tr("Cancel"));
  aBtn->setObjectName(PROP_PANEL_CANCEL);
  aBtn->setFlat(true);
  aBtn->setShortcut(QKeySequence(Qt::Key_Escape));
  aBtnLay->addWidget(aBtn);

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

      QPushButton* anOkBtn = findChild<QPushButton*>(PROP_PANEL_OK);
      QPushButton* aCancelBtn = findChild<QPushButton*>(PROP_PANEL_CANCEL);

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
  if(!theFeature->data())
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

void XGUI_PropertyPanel::setAcceptEnabled(bool isEnabled)
{
  QPushButton* anOkBtn = findChild<QPushButton*>(PROP_PANEL_OK);
  anOkBtn->setEnabled(true);//isEnabled);
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
    theWidget->activate();
    theWidget->setHighlighted(true);
  }
  myActiveWidget = theWidget;
  if (myActiveWidget)
    emit widgetActivated(theWidget);
  else if (!isEditingMode())
    emit noMoreWidgets();
}

void XGUI_PropertyPanel::setOkEnabled(bool theEnabled)
{
  QPushButton* anOkBtn = findChild<QPushButton*>(PROP_PANEL_OK);
  anOkBtn->setEnabled(true);//theEnabled);
}

bool XGUI_PropertyPanel::isOkEnabled() const
{
  QPushButton* anOkBtn = findChild<QPushButton*>(PROP_PANEL_OK);
  return anOkBtn->isEnabled();
}

void XGUI_PropertyPanel::setCancelEnabled(bool theEnabled)
{
  QPushButton* anCancelBtn = findChild<QPushButton*>(PROP_PANEL_CANCEL);
  anCancelBtn->setEnabled(theEnabled);
}

bool XGUI_PropertyPanel::isCancelEnabled() const
{
  QPushButton* anCancelBtn = findChild<QPushButton*>(PROP_PANEL_CANCEL);
  return anCancelBtn->isEnabled();
}


