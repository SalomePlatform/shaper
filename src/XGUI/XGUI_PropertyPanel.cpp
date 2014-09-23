/*
 * XGUI_PropertyPanel.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: sbh
 */

#include <XGUI_PropertyPanel.h>
#include <XGUI_Constants.h>
#include <ModuleBase_WidgetPoint2D.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>
#include <QEvent>
#include <QKeyEvent>

#ifdef _DEBUG
#include <iostream>
#endif

XGUI_PropertyPanel::XGUI_PropertyPanel(QWidget* theParent)
    : QDockWidget(theParent)
{
  this->setWindowTitle(tr("Property Panel"));
  QAction* aViewAct = this->toggleViewAction();
  this->setObjectName(XGUI::PROP_PANEL);
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
  aBtn->setObjectName(XGUI::PROP_PANEL_OK);
  aBtn->setToolTip(tr("Ok"));
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);

  aBtn = new QPushButton(QIcon(":pictures/button_cancel.png"), "", aFrm);
  aBtn->setToolTip(tr("Cancel"));
  aBtn->setObjectName(XGUI::PROP_PANEL_CANCEL);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);

  myCustomWidget = new QWidget(aContent);
  myMainLayout->addWidget(myCustomWidget);
  myMainLayout->addStretch(1);
}

XGUI_PropertyPanel::~XGUI_PropertyPanel()
{
}

void XGUI_PropertyPanel::cleanContent()
{
  myWidgets.clear();
  qDeleteAll(myCustomWidget->children());
}

void XGUI_PropertyPanel::setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets)
{
  myWidgets = theWidgets;
  if (theWidgets.empty()) return;

  QList<ModuleBase_ModelWidget*>::const_iterator anIt = theWidgets.begin(), aLast =
      theWidgets.end();
  for (; anIt != aLast; anIt++) {
    connect(*anIt, SIGNAL(keyReleased(QKeyEvent*)), this, SIGNAL(keyReleased(QKeyEvent*)));

    connect(*anIt, SIGNAL(focusOutWidget(ModuleBase_ModelWidget*)), this,
            SLOT(onActivateNextWidget(ModuleBase_ModelWidget*)));
    connect(*anIt, SIGNAL(focusInWidget(ModuleBase_ModelWidget*)),
            this, SIGNAL(widgetActivated(ModuleBase_ModelWidget*)));

    ModuleBase_WidgetPoint2D* aPointWidget = dynamic_cast<ModuleBase_WidgetPoint2D*>(*anIt);
    if (aPointWidget)
      connect(aPointWidget, SIGNAL(storedPoint2D(ObjectPtr, const std::string&)), this,
              SIGNAL(storedPoint2D(ObjectPtr, const std::string&)));
  }
  ModuleBase_ModelWidget* aLastWidget = theWidgets.last();
  if (aLastWidget) {
    QList<QWidget*> aControls = aLastWidget->getControls();
    if (!aControls.empty()) {
      QWidget* aLastControl = aControls.last();

      QPushButton* anOkBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
      QPushButton* aCancelBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_CANCEL);

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
  foreach(ModuleBase_ModelWidget* eachWidget, myWidgets)
  {
    eachWidget->setFeature(theFeature);
    eachWidget->restoreValue();
  }
  // the repaint is used here to immediatelly react in GUI to the values change.
  repaint();
}

void XGUI_PropertyPanel::onActivateNextWidget(ModuleBase_ModelWidget* theWidget)
{
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
  emit widgetActivated(aNextWidget);
}

void XGUI_PropertyPanel::setAcceptEnabled(bool isEnabled)
{
  QPushButton* anOkBtn = findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
  anOkBtn->setEnabled(isEnabled);
}
