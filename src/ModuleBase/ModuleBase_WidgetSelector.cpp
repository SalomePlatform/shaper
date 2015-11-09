// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetSelector.cpp
// Created:     19 June 2015
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetSelector.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_ResultConstruction.h>

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

  bool isDone = setSelection(aSelected, true/*false*/);
  // "false" flag should be used here, it connects to the #26658 OCC bug, when the user click in 
  // the same place repeatedly without mouse moved. In the case validation by filters is not
  // perfromed, so an invalid object is selected. E.g. distance constraint, selection of a point.
  // the 3rd click in the same point allow using this point.
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

  QIntList aShapeTypes = getShapeTypes();
  if (aShapeTypes.empty()) {
    aValid = true;
    return aValid;
  }
  // when the SHAPE type is provided by XML, the hole result shape can be selected.
  if (!aShape.get() && aShapeTypes.contains(TopAbs_SHAPE)) {
    aValid = true;
    return aValid;
  }

  if (!aShape.get() && theResult.get()) {
    if (theResult.get())
      aShape = theResult->shape();
  }
  TopAbs_ShapeEnum aShapeType = TopAbs_SHAPE;
  if (aShape.get()) {
    // Check that the selection corresponds to selection type
    TopoDS_Shape aTopoShape = aShape->impl<TopoDS_Shape>();
    aShapeType = aTopoShape.ShapeType();
    // for compounds check sub-shapes: it may be compound of needed type:
    // Booleans may produce compounds of Solids
    if (aShapeType == TopAbs_COMPOUND) {
      aShapeType = ModuleBase_Tools::getCompoundSubType(aTopoShape);
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
void ModuleBase_WidgetSelector::activateSelectionAndFilters(bool toActivate)
{
  updateSelectionName();

  if (toActivate) {
    myWorkshop->activateSubShapesSelection(getShapeTypes());
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }
  activateFilters(toActivate);
}

//********************************************************************
void ModuleBase_WidgetSelector::activateCustom()
{
  connect(myWorkshop, SIGNAL(selectionChanged()), this,
          SLOT(onSelectionChanged()), Qt::UniqueConnection);
  
  activateSelectionAndFilters(true);

  // Restore selection in the viewer by the attribute selection list
  myWorkshop->setSelected(getAttributeSelection());
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
  ModuleBase_ModelWidget::deactivate();
  disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  activateSelectionAndFilters(false);
  ModuleBase_ModelWidget::deactivate();
}

