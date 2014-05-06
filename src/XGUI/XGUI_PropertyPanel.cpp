/*
 * XGUI_PropertyPanel.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: sbh
 */

#include <XGUI_Constants.h>
#include <XGUI_PropertyPanel.h>

#include <ModuleBase_PropPanelOperation.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>

#ifdef _DEBUG
#include <iostream>
#endif

XGUI_PropertyPanel::XGUI_PropertyPanel(QWidget* theParent)
{
  this->setWindowTitle(tr("Property Panel"));
  QAction* aViewAct = this->toggleViewAction();
  this->setObjectName(XGUI::PROP_PANEL);

  QWidget* aContent = new QWidget(this);
  QVBoxLayout* aMainLay = new QVBoxLayout(aContent);
  aMainLay->setContentsMargins(3, 3, 3, 3);
  this->setWidget(aContent);

  QFrame* aFrm = new QFrame(aContent);
  aFrm->setFrameStyle(QFrame::Sunken);
  aFrm->setFrameShape(QFrame::Panel);
  QHBoxLayout* aBtnLay = new QHBoxLayout(aFrm);
  aBtnLay->setContentsMargins(0, 0, 0, 0);
  aMainLay->addWidget(aFrm);

  QPushButton* aBtn = new QPushButton(QIcon(":pictures/button_help.png"), "", aFrm);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);
  aBtnLay->addStretch(1);
  aBtn = new QPushButton(QIcon(":pictures/button_ok.png"), "", aFrm);
  aBtn->setObjectName(XGUI::PROP_PANEL_OK);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);
  aBtn = new QPushButton(QIcon(":pictures/button_cancel.png"), "", aFrm);
  aBtn->setObjectName(XGUI::PROP_PANEL_CANCEL);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);

  myCustomWidget = new QWidget(aContent);
  aMainLay->addWidget(myCustomWidget);
  aMainLay->addStretch(1);
}

XGUI_PropertyPanel::~XGUI_PropertyPanel()
{
}

void XGUI_PropertyPanel::setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets)
{
  myWidgets = theWidgets;
}

QWidget* XGUI_PropertyPanel::contentWidget()
{
  return myCustomWidget;
}

void XGUI_PropertyPanel::updateContentWidget(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  foreach(ModuleBase_ModelWidget* eachWidget, myWidgets) {
    eachWidget->restoreValue(theFeature);
  }
  // the repaint is used here to immediatelly react in GUI to the values change.
  repaint();
}
