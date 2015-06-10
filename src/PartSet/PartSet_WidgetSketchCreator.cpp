// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchCreator.cpp
// Created:     08 June 2015
// Author:      Vitaly SMETANNIKOV

#include "PartSet_WidgetSketchCreator.h"
#include "PartSet_Module.h"

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>

#include <GeomAPI_Face.h>

#include <ModelAPI_Session.h>

#include <ModuleBase_Tools.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_IPropertyPanel.h>
#include <Config_WidgetAPI.h>

#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>

PartSet_WidgetSketchCreator::PartSet_WidgetSketchCreator(QWidget* theParent, 
                                                         PartSet_Module* theModule,
                                                         const Config_WidgetAPI* theData,
                                                         const std::string& theParentId)
: ModuleBase_ModelWidget(theParent, theData, theParentId), myModule(theModule)
{
  QFormLayout* aLayout = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aLayout);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, this);
  if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(QPixmap(aLabelIcon));


  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  myTextLine = new QLineEdit(this);
  myTextLine->setReadOnly(true);
  myTextLine->setToolTip(aToolTip);
  myTextLine->installEventFilter(this);

  aLayout->addRow(myLabel, myTextLine);
}

PartSet_WidgetSketchCreator::~PartSet_WidgetSketchCreator()
{
}

QList<QWidget*> PartSet_WidgetSketchCreator::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myTextLine);
  return aControls;
}

bool PartSet_WidgetSketchCreator::restoreValue()
{
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aCompFeature->numberOfSubs() > 0) {
    FeaturePtr aSubFeature = aCompFeature->subFeature(0);
    myTextLine->setText(QString::fromStdString(aSubFeature->data()->name()));
  }
  return true;
}

bool PartSet_WidgetSketchCreator::storeValueCustom() const
{
  return true;
}

void PartSet_WidgetSketchCreator::activateCustom()
{
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aCompFeature->numberOfSubs() == 0)
    connect(myModule, SIGNAL(operationLaunched()), SLOT(onStarted()));
}

void PartSet_WidgetSketchCreator::onStarted()
{
  disconnect(myModule, SIGNAL(operationLaunched()), this, SLOT(onStarted()));

  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  FeaturePtr aSketch = aCompFeature->addFeature("Sketch");

  ModuleBase_Operation* anOperation = myModule->createOperation("Sketch");
  anOperation->setFeature(aSketch);
  myModule->sendOperation(anOperation);
}

bool PartSet_WidgetSketchCreator::focusTo()
{
  CompositeFeaturePtr aCompFeature = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  if (aCompFeature->numberOfSubs() == 0)
    return ModuleBase_ModelWidget::focusTo(); 

  SessionPtr aMgr = ModelAPI_Session::get();
  bool aIsOp = aMgr->isOperation();
  // Open transaction if it was closed before
  if (!aIsOp)
    aMgr->startOperation();

  restoreValue();
  return false;
}