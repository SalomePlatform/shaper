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
  return true;
}

bool PartSet_WidgetSketchCreator::storeValueCustom() const
{
  return true;
}

void PartSet_WidgetSketchCreator::activateCustom()
{
  connect(myModule, SIGNAL(operationLaunched()), SLOT(onStarted()));


  //XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  //XGUI_Workshop* aWorkshop = aConnector->workshop();
  //XGUI_Displayer* aDisp = aWorkshop->displayer();

  //QIntList aModes;
  //aModes << TopAbs_FACE;
  //aDisp->activateObjects(aModes);
  //  
  //connect(aWorkshop->selector(), SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  //activateFilters(myModule->workshop(), true);
}

void PartSet_WidgetSketchCreator::onStarted()
{
  disconnect(myModule, SIGNAL(operationLaunched()), this, SLOT(onStarted()));

  CompositeFeaturePtr aCompFeature = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  FeaturePtr aSketch = aCompFeature->addFeature("Sketch");

  ModuleBase_Operation* anOperation = myModule->createOperation("Sketch");
  anOperation->setFeature(aSketch);
  myModule->sendOperation(anOperation);
}

void PartSet_WidgetSketchCreator::storeAttributeValue()
{
}

void PartSet_WidgetSketchCreator::restoreAttributeValue(const bool theValid)
{
}

bool PartSet_WidgetSketchCreator::setSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face());
  aFace->setImpl(new TopoDS_Shape(thePrs.shape()));
  if (aFace->isPlanar())
    return true;
  //CompositeFeaturePtr aCompFeature = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  //FeaturePtr aSketch = aCompFeature->addFeature("Sketch");

  //ModuleBase_Operation* anOperation = myModule->createOperation("Sketch");
  //anOperation->setFeature(aSketch);
  //myModule->sendOperation(anOperation);
  return false;
}

bool PartSet_WidgetSketchCreator::setSelection(const QList<ModuleBase_ViewerPrs>& theValues, int& thePosition)
{
  return true;
}