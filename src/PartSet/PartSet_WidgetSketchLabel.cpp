// File:        PartSet_WidgetSketchLabel.cpp
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_OperationSketch.h"

#include <ModuleBase_Operation.h>
#include <XGUI_OperationMgr.h>

#include <Config_WidgetAPI.h>

#include <QLabel>

PartSet_WidgetSketchLabel::PartSet_WidgetSketchLabel(QWidget* theParent,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  myText = QString::fromStdString(theData->getProperty("title"));
  myLabel = new QLabel("", theParent);
  myLabel->setWordWrap(true);
  myTooltip = QString::fromStdString(theData->getProperty("tooltip"));
  myLabel->setToolTip("");
  myLabel->setIndent(5);
}

QList<QWidget*> PartSet_WidgetSketchLabel::getControls() const
{
  QList<QWidget*> aLst;
  aLst << myLabel;
  return aLst;
}

QWidget* PartSet_WidgetSketchLabel::getControl() const
{
  return myLabel;
}

void PartSet_WidgetSketchLabel::setOperationsMgr(XGUI_OperationMgr* theMgr)
{
  ModuleBase_Operation* aOperation = theMgr->currentOperation();
  if (aOperation->inherits("PartSet_OperationSketch")) {
    PartSet_OperationSketch* aSketchOpe = static_cast<PartSet_OperationSketch*>(aOperation);
    updateLabel(aSketchOpe);
    connect(aSketchOpe, SIGNAL(planeSelected(double, double, double)), this,
            SLOT(onPlaneSelected()));
  }
}

void PartSet_WidgetSketchLabel::onPlaneSelected()
{
  PartSet_OperationSketch* aSketchOpe = static_cast<PartSet_OperationSketch*>(sender());
  updateLabel(aSketchOpe);
}

void PartSet_WidgetSketchLabel::updateLabel(PartSet_OperationSketch* theSketchOpe)
{
  if (theSketchOpe->hasSketchPlane()) {
    myLabel->setText("");
    myLabel->setToolTip("");
  } else {
    myLabel->setText(myText);
    myLabel->setToolTip(myTooltip);
  }
}
