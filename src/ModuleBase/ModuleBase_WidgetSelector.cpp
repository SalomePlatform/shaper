// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetSelector.h
// Created:     19 June 2015
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetSelector.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>

ModuleBase_WidgetSelector::ModuleBase_WidgetSelector(QWidget* theParent,
                                                     ModuleBase_IWorkshop* theWorkshop,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
 : ModuleBase_WidgetValidated(theParent, theData, theParentId),
   myWorkshop(theWorkshop)
{
}

//********************************************************************
ModuleBase_WidgetSelector::~ModuleBase_WidgetSelector()
{
}

//TODO: nds stabilization hotfix
void ModuleBase_WidgetSelector::disconnectSignals()
{
  disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
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

  QList<ModuleBase_ViewerPrs> aSelected = myWorkshop->selection()->getSelected(
                                                              ModuleBase_ISelection::AllControls);
  setSelection(aSelected);

  emit valuesChanged();
  // the updateObject method should be called to flush the updated sigal. The workshop listens it,
  // calls validators for the feature and, as a result, updates the Apply button state.
  updateObject(myFeature);

  updateFocus();
}

//********************************************************************
bool ModuleBase_WidgetSelector::acceptSubShape(const TopoDS_Shape& theShape) const
{
  bool aValid = true;
  if (theShape.IsNull()) {
    aValid = true; // do not check the shape type if the shape is empty
    // extrusion uses a sketch object selectected in Object browser
  }
  else {
    aValid = false;
    TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
    QIntList aShapeTypes = getShapeTypes();

    QIntList::const_iterator anIt = aShapeTypes.begin(), aLast = aShapeTypes.end();
    for (; anIt != aLast; anIt++) {
      if (aShapeType == *anIt)
        aValid = true;
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

  activateFilters(myWorkshop, true);
}

//********************************************************************
bool ModuleBase_WidgetSelector::isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  GeomShapePtr aShape = myWorkshop->selection()->getShape(thePrs);
  bool aValid;
  // if there is no selected shape, the method returns true
  if (!aShape.get())
    aValid = true;
  else {
    // Check that the selection corresponds to selection type
    TopoDS_Shape aTopoShape = aShape->impl<TopoDS_Shape>();
    aValid = acceptSubShape(aTopoShape);
  }
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
  activateFilters(myWorkshop, false);
}
