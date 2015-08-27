// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetSelector.cpp
// Created:     19 June 2015
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetSelector.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_ResultConstruction.h>

#include <GeomValidators_Tools.h>

#include <TopoDS_Iterator.hxx>

ModuleBase_WidgetSelector::ModuleBase_WidgetSelector(QWidget* theParent,
                                                     ModuleBase_IWorkshop* theWorkshop,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
 : ModuleBase_WidgetValidated(theParent, theWorkshop, theData, theParentId)
{
}

//********************************************************************
ModuleBase_WidgetSelector::~ModuleBase_WidgetSelector()
{
}

//********************************************************************
void ModuleBase_WidgetSelector::getGeomSelection(const ModuleBase_ViewerPrs& thePrs,
                                                      ObjectPtr& theObject,
                                                      GeomShapePtr& theShape)
{
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  theObject = aSelection->getResult(thePrs);
  theShape = aSelection->getShape(thePrs);
}

//********************************************************************
void ModuleBase_WidgetSelector::onSelectionChanged()
{
  clearAttribute();

  QList<ModuleBase_ViewerPrs> aSelected = getFilteredSelected();

  bool isDone = setSelection(aSelected, false);
  emit valuesChanged();
  // the updateObject method should be called to flush the updated sigal. The workshop listens it,
  // calls validators for the feature and, as a result, updates the Apply button state.
  updateObject(myFeature);

  if (isDone)
    updateFocus();
}

//********************************************************************
bool ModuleBase_WidgetSelector::acceptSubShape(const GeomShapePtr& theShape,
                                               const ResultPtr& theResult) const
{
  bool aValid = false;

  GeomShapePtr aShape = theShape;
  if (!aShape.get() && theResult.get()) {
    if (theResult.get())
      aShape = theResult->shape();
  }
  QIntList aShapeTypes = getShapeTypes();

  TopAbs_ShapeEnum aShapeType = TopAbs_SHAPE;
  if (aShape.get()) {
    // Check that the selection corresponds to selection type
    TopoDS_Shape aTopoShape = aShape->impl<TopoDS_Shape>();
    aShapeType = aTopoShape.ShapeType();
    // for compounds check sub-shapes: it may be compound of needed type:
    // Booleans may produce compounds of Solids
    if (aShapeType == TopAbs_COMPOUND) {
      if (aShapeTypes.contains(aShapeType))
        return true;

      aShapeType = GeomValidators_Tools::getCompoundSubType(aTopoShape);
    }
  }

  QIntList::const_iterator anIt = aShapeTypes.begin(), aLast = aShapeTypes.end();
  for (; anIt != aLast; anIt++) {
    if (aShapeType == *anIt)
      aValid = true;
    else if (*anIt == TopAbs_FACE) {
      // try to process the construction shape only if there is no a selected shape in the viewer
      if (!theShape.get() && theResult.get()) {
        ResultConstructionPtr aCResult =
                                std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theResult);
        aValid = aCResult.get() && aCResult->facesNum() > 0;
      }
    }
  }
  return aValid;
}

//********************************************************************
void ModuleBase_WidgetSelector::activateSelection(bool toActivate)
{
  updateSelectionName();

  if (toActivate) {
    myWorkshop->activateSubShapesSelection(getShapeTypes());
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }
}

//********************************************************************
void ModuleBase_WidgetSelector::activateCustom()
{
  connect(myWorkshop, SIGNAL(selectionChanged()), this,
          SLOT(onSelectionChanged()), Qt::UniqueConnection);
  
  activateSelection(true);

  // Restore selection in the viewer by the attribute selection list
  myWorkshop->setSelected(getAttributeSelection());

  activateFilters(true);
}

//********************************************************************
bool ModuleBase_WidgetSelector::isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  GeomShapePtr aShape = myWorkshop->selection()->getShape(thePrs);
  ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
  bool aValid = acceptSubShape(aShape, aResult);

  if (aValid) {
    // In order to avoid selection of the same object
    ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
    FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aResult);
    aValid = aSelectedFeature != myFeature;
  }
  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetSelector::setSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  ObjectPtr anObject;
  GeomShapePtr aShape;
  getGeomSelection(thePrs, anObject, aShape);

  setObject(anObject, aShape);
  return true;
}

//********************************************************************
void ModuleBase_WidgetSelector::deactivate()
{
  disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  activateSelection(false);
  activateFilters(false);
}

