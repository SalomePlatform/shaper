// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetShapeSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_Filter.h>

#include <Config_WidgetAPI.h>
#include <Events_Loop.h>
#include <Events_Message.h>
#include <GeomAPI_Interface.h>
#include <GeomAPI_Shape.h>
#include <GeomValidators_Tools.h>

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_ShapeValidator.h>

#include <Config_WidgetAPI.h>
#include <Events_Error.h>

#include <GeomAPI_Shape.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QString>
#include <QEvent>
#include <QDockWidget>
#include <QApplication>
#include <QFormLayout>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <memory>

#include <list>
#include <string>

typedef QMap<QString, TopAbs_ShapeEnum> ShapeTypes;
static ShapeTypes MyShapeTypes;

TopAbs_ShapeEnum ModuleBase_WidgetShapeSelector::shapeType(const QString& theType)
{
  if (MyShapeTypes.count() == 0) {
    MyShapeTypes["face"] = TopAbs_FACE;
    MyShapeTypes["faces"] = TopAbs_FACE;
    MyShapeTypes["vertex"] = TopAbs_VERTEX;
    MyShapeTypes["vertices"] = TopAbs_VERTEX;
    MyShapeTypes["wire"] = TopAbs_WIRE;
    MyShapeTypes["edge"] = TopAbs_EDGE;
    MyShapeTypes["edges"] = TopAbs_EDGE;
    MyShapeTypes["shell"] = TopAbs_SHELL;
    MyShapeTypes["solid"] = TopAbs_SOLID;
    MyShapeTypes["solids"] = TopAbs_SOLID;
  }
  QString aType = theType.toLower();
  if (MyShapeTypes.contains(aType))
    return MyShapeTypes[aType];
  Events_Error::send("Shape type defined in XML is not implemented!");
  return TopAbs_SHAPE;
}

ModuleBase_WidgetShapeSelector::ModuleBase_WidgetShapeSelector(QWidget* theParent,
                                                     ModuleBase_IWorkshop* theWorkshop,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
    : ModuleBase_WidgetValidated(theParent, theData, theParentId),
      myWorkshop(theWorkshop), myIsActive(false)
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

  std::string aTypes = theData->getProperty("shape_types");
  myShapeTypes = QString(aTypes.c_str()).split(' ', QString::SkipEmptyParts);
}

//********************************************************************
ModuleBase_WidgetShapeSelector::~ModuleBase_WidgetShapeSelector()
{
  activateSelection(false);
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::storeValueCustom() const
{
  // the value is stored on the selection changed signal processing 
  return true;
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::storeAttributeValues(ObjectPtr theSelectedObject,
                                                          GeomShapePtr theShape)
{
  bool isChanged = false;
  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(theSelectedObject);
  if (aSelectedFeature == myFeature)  // In order to avoid selection of the same object
    return isChanged;

  DataPtr aData = myFeature->data();
  AttributeReferencePtr aRef = aData->reference(attributeID());
  if (aRef) {
    ObjectPtr aObject = aRef->value();
    if (!(aObject && aObject->isSame(theSelectedObject))) {
      aRef->setValue(theSelectedObject);
      isChanged = true;
    }
  } else {
    AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
    if (aRefAttr) {
      ObjectPtr aObject = aRefAttr->object();
      if (!(aObject && aObject->isSame(theSelectedObject))) {
        aRefAttr->setObject(theSelectedObject);
        isChanged = true;
      }
    } else {
      AttributeSelectionPtr aSelectAttr = aData->selection(attributeID());
      ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(theSelectedObject);
      if (aSelectAttr && aBody && (theShape.get() != NULL)) {
        aSelectAttr->setValue(aBody, theShape);
        isChanged = true;
      }
    }
  }
  return isChanged;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::clearAttribute()
{
  DataPtr aData = myFeature->data();
  AttributeSelectionPtr aSelect = aData->selection(attributeID());
  if (aSelect) {
    aSelect->setValue(ResultPtr(), std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape()));
    return;
  }
  AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
  if (aRefAttr) {
    aRefAttr->setObject(ObjectPtr());
    return;
  }
  AttributeReferencePtr aRef = aData->reference(attributeID());
  if (aRef) {
    aRef->setObject(ObjectPtr());
  }
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::restoreValue()
{
  bool isBlocked = this->blockSignals(true);
  updateSelectionName();
  this->blockSignals(isBlocked);

  return true;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetShapeSelector::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myTextLine);
  return aControls;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::onSelectionChanged()
{
  // In order to make reselection possible
  // TODO: check with MPV clearAttribute();

  //QObjectPtrList aObjects = myWorkshop->selection()->selectedPresentations();
  QList<ModuleBase_ViewerPrs> aSelected = myWorkshop->selection()->getSelected();
  if (aSelected.size() > 0) {
    Handle(SelectMgr_EntityOwner) anOwner = aSelected.first().owner();
    if (isValid(anOwner)) {
      setSelection(anOwner);
      // the updateObject method should be called to flush the updated sigal. The workshop listens it,
      // calls validators for the feature and, as a result, updates the Apply button state.
      updateObject(myFeature);
      //if (theObj) {
        //  raisePanel();
      //} 
      //updateSelectionName();
      //emit valuesChanged();
      emit focusOutWidget(this);
    }
  }
}

//********************************************************************
//bool ModuleBase_WidgetShapeSelector::acceptObjectShape(const ObjectPtr theResult) const
//{
//  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theResult);
//
//  // Check that the shape of necessary type
//  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
//  if (!aShapePtr)
//    return false;
//  TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
//  if (aShape.IsNull())
//    return false;
//
//  TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
//  if (aShapeType == TopAbs_COMPOUND) {
//    foreach (QString aType,
//      myShapeTypes) {
//      TopExp_Explorer aEx(aShape, shapeType(aType));
//      if (aEx.More())
//        return true;
//    }
//  } else {
//    foreach (QString aType, myShapeTypes) {
//      if (shapeType(aType) == aShapeType)
//        return true;
//    }
//  }
//  return false;
//}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::acceptSubShape(std::shared_ptr<GeomAPI_Shape> theShape) const
{
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  foreach (QString aType, myShapeTypes) {
    if (aShape.ShapeType() == shapeType(aType))
      return true;
  }
  return false;
}

//********************************************************************
GeomShapePtr ModuleBase_WidgetShapeSelector::getShape() const
{
  GeomShapePtr aShape;
  DataPtr aData = myFeature->data();
  if (aData.get() == NULL)
    return aShape;

  AttributeSelectionPtr aSelect = aData->selection(attributeID());
  if (aSelect)
    aShape = aSelect->value();

  return aShape;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::updateSelectionName()
{
  DataPtr aData = myFeature->data();
  if (aData.get() == NULL)
    return;

  bool isNameUpdated = false;
  AttributeSelectionPtr aSelect = aData->selection(attributeID());
  if (aSelect) {
    myTextLine->setText(QString::fromStdString(aSelect->namingName()));
    isNameUpdated = true;
  }
  if (!isNameUpdated) {
    ObjectPtr anObject = GeomValidators_Tools::getObject(myFeature->attribute(attributeID()));
    if (anObject.get() != NULL) {
      std::string aName = anObject->data()->name();
      myTextLine->setText(QString::fromStdString(aName));
    } else {
      AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
      if (aRefAttr && aRefAttr->attr().get() != NULL) {
        //myIsObject = aRefAttr->isObject();
        AttributePtr anAttr = aRefAttr->attr();
        if (anAttr.get() != NULL) {
          std::stringstream aName;
          aName <<anAttr->owner()->data()->name()<<"/"<<anAttr->id();
          myTextLine->setText(QString::fromStdString(aName.str()));
        }
      }
      else if (myIsActive) {
        myTextLine->setText("");
      }
    }
  }
}


//********************************************************************
void ModuleBase_WidgetShapeSelector::activateSelection(bool toActivate)
{
  if (myIsActive == toActivate)
    return;
  myIsActive = toActivate;
  updateSelectionName();

  if (myIsActive) {
    QIntList aList;
    foreach (QString aType, myShapeTypes) {
      aList.append(shapeType(aType));
    }
    myWorkshop->activateSubShapesSelection(aList);
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }

  activateFilters(myWorkshop, myIsActive);
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::raisePanel() const
{
  QWidget* aParent = this->parentWidget();
  QWidget* aLastPanel = 0;
  while (!aParent->inherits("QDockWidget")) {
    aLastPanel = aParent;
    aParent = aParent->parentWidget();
    if (!aParent)
      return;
  }
  if (aParent->inherits("QDockWidget")) {
    QDockWidget* aTabWgt = (QDockWidget*) aParent;
    aTabWgt->raise();
  }
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::activateCustom()
{
  connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  activateSelection(true);
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::backupAttributeValue(const bool isBackup)
{
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());

  if (isBackup) {
    myObject = GeomValidators_Tools::getObject(anAttribute);
    myShape = getShape();
    myRefAttribute = NULL;
    myIsObject = false;
    AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
    if (aRefAttr) {
      myIsObject = aRefAttr->isObject();
      myRefAttribute = aRefAttr->attr();
    }
    myExternalObject = NULL;
  }
  else {
    storeAttributeValues(myObject, myShape);
    AttributeRefAttrPtr aRefAttr = aData->refattr(attributeID());
    if (aRefAttr) {
      if (!myIsObject)
        aRefAttr->setAttr(myRefAttribute);
    }
    if (myExternalObject.get()) {
      /*DocumentPtr aDoc = myExternalObject->document();
      FeaturePtr aFeature = ModelAPI_Feature::feature(myExternalObject);
      if (aFeature.get() != NULL) {
        aDoc->removeFeature(aFeature);
      }*/
    }
  }
}

//********************************************************************
bool ModuleBase_WidgetShapeSelector::setSelection(const Handle_SelectMgr_EntityOwner& theOwner)
{
  bool isDone = false;

  ModuleBase_ViewerPrs aPrs;
  myWorkshop->selection()->fillPresentation(aPrs, theOwner);
  ObjectPtr aObject = aPrs.object();
  ObjectPtr aCurrentObject = GeomValidators_Tools::getObject(myFeature->attribute(attributeID()));
  if ((!aCurrentObject) && (!aObject))
    return false;

  // Check that the selected object is result (others can not be accepted)
  ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObject);
  if (!aRes)
    return false;

  if (myFeature) {
    // We can not select a result of our feature
    const std::list<std::shared_ptr<ModelAPI_Result>>& aResList = myFeature->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aIt;
    for (aIt = aResList.cbegin(); aIt != aResList.cend(); ++aIt) {
      if ((*aIt) == aRes)
        return false;
    }
  }
  // Check that object belongs to active document or PartSet
  DocumentPtr aDoc = aRes->document();
  SessionPtr aMgr = ModelAPI_Session::get();
  if (!(aDoc == aMgr->activeDocument()) && !(aDoc == aMgr->moduleDocument()))
    return false;

  // Check that the result has a shape
  GeomShapePtr aShape = ModelAPI_Tools::shape(aRes);
  if (!aShape)
    return false;

  // Get sub-shapes from local selection
  if (!aPrs.shape().IsNull()) {
    aShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aPrs.shape()));
  }
  // Check that the selection corresponds to selection type
  if (!acceptSubShape(aShape))
    return false;

  storeAttributeValues(aObject, aShape);
  return true;
}

//********************************************************************
void ModuleBase_WidgetShapeSelector::deactivate()
{
  activateSelection(false);
  disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}
