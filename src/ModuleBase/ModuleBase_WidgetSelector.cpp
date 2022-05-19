// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <ModuleBase_WidgetSelector.h>

#include <ModuleBase_Events.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_ISelectionActivate.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_ResultPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_WidgetFactory.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultGroup.h>

#include <Config_WidgetAPI.h>

#include <TopoDS_Iterator.hxx>

// Get object from group
// Return true if find object
static bool getObjectFromGroup(ObjectPtr& theObject, GeomShapePtr& theShape);

ModuleBase_WidgetSelector::ModuleBase_WidgetSelector(QWidget* theParent,
                                                     ModuleBase_IWorkshop* theWorkshop,
                                                     const Config_WidgetAPI* theData)
: ModuleBase_WidgetValidated(theParent, theWorkshop, theData),
myIsPointsFiltering(true)
{
  QString aFiltering = QString::fromStdString(theData->getProperty("filter_points"));
  if (aFiltering.toLower() == "false")
    myIsPointsFiltering = false;
}

//********************************************************************
ModuleBase_WidgetSelector::~ModuleBase_WidgetSelector()
{
}

//********************************************************************
void ModuleBase_WidgetSelector::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                      ObjectPtr& theObject,
                                                      GeomShapePtr& theShape)
{
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  theObject = aSelection->getResult(thePrs);
  if (!theObject.get())
    theObject = thePrs->object();
  theShape = aSelection->getShape(thePrs);

  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  while (aFeature.get() &&
         aFeature->lastResult().get() &&
         aFeature->lastResult()->groupName() == ModelAPI_ResultGroup::group()) {
    if (!getObjectFromGroup(theObject, theShape))
      break;
    aFeature = ModelAPI_Feature::feature(theObject);

    thePrs->setObject(theObject);
  }
}

//********************************************************************
bool ModuleBase_WidgetSelector::processSelection()
{
  QList<ModuleBase_ViewerPrsPtr> aSelected = getFilteredSelected();
  // equal vertices should not be used here
  if (myIsPointsFiltering)
    ModuleBase_ISelection::filterSelectionOnEqualPoints(aSelected);

  bool isDone = setSelection(aSelected, true/*false*/);
  updateOnSelectionChanged(isDone);

  return isDone;
}

//********************************************************************
void ModuleBase_WidgetSelector::updateOnSelectionChanged(const bool theDone)
{
  // "false" flag should be used here, it connects to the #26658 OCC bug, when the user click in
  // the same place repeatedly without mouse moved. In the case validation by filters is not
  // perfromed, so an invalid object is selected. E.g. distance constraint, selection of a point.
  // the 3rd click in the same point allow using this point.
  emit valuesChanged();
  // the updateObject method should be called to flush the updated sigal. The workshop listens it,
  // calls validators for the feature and, as a result, updates the Apply button state.
  updateObject(myFeature);

  // we need to forget about previous validation result as the current selection can influence on it
  clearValidatedCash();

  if (theDone)
    updateFocus();
}

//********************************************************************
QIntList ModuleBase_WidgetSelector::getShapeTypes() const
{
  QIntList aShapeTypes = shapeTypes();
  // this type should be mentioned in XML, poor selection otherwise
  if (/*aShapeTypes.contains(TopAbs_SOLID) ||*/
      aShapeTypes.contains(ModuleBase_ResultPrs::Sel_Result/*TopAbs_SHAPE*/)) {
    // it should be selectable for both, "solids" and "objects" types
    aShapeTypes.append(TopAbs_COMPSOLID);
  }
  return aShapeTypes;
}

//********************************************************************
QList<ModuleBase_ViewerPrsPtr> ModuleBase_WidgetSelector::getAttributeSelection() const
{
  return QList<ModuleBase_ViewerPrsPtr>();
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
  // when the SHAPE type is provided by XML as Object, the whole result shape should be selected.
  //if (!aShape.get() && aShapeTypes.contains(ModuleBase_ResultPrs::Sel_Result)) {
  // In case of selection of a feature aShape could be not NULL, but result has to be selected
  if (aShapeTypes.contains(ModuleBase_ResultPrs::Sel_Result)) {
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
    if (!aTopoShape.IsNull()) {
      aShapeType = aTopoShape.ShapeType();
      // for compounds check sub-shapes: it may be compound of needed type:
      // Booleans may produce compounds of Solids
      if (aShapeType == TopAbs_COMPOUND) {
        aShapeType = ModuleBase_Tools::getCompoundSubType(aTopoShape);
      }
    }
  }

  QIntList::const_iterator anIt = aShapeTypes.begin(), aLast = aShapeTypes.end();
  for (; anIt != aLast && !aValid; anIt++) {
    TopAbs_ShapeEnum aCurrentShapeType = (TopAbs_ShapeEnum)*anIt;
    if (aShapeType == aCurrentShapeType)
      aValid = true;
    else if (aCurrentShapeType == TopAbs_FACE) {
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
void ModuleBase_WidgetSelector::selectionModes(int& theModuleSelectionModes, QIntList& theModes)
{
  theModuleSelectionModes = -1;
  theModes.append(getShapeTypes());
}

//********************************************************************
void ModuleBase_WidgetSelector::updateSelectionModesAndFilters(bool toActivate)
{
  updateSelectionName();

  myWorkshop->selectionActivate()->updateSelectionFilters();
  myWorkshop->selectionActivate()->updateSelectionModes();

  if (!toActivate)
    clearValidatedCash();
}

//********************************************************************
void ModuleBase_WidgetSelector::activateCustom()
{
  // Restore selection in the viewer by the attribute selection list
  // it should be postponed to have current widget as active to validate restored selection
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_UPDATE_BY_WIDGET_SELECTION);
  ModelAPI_EventCreator::get()->sendUpdated(myFeature, anEvent);
}

//********************************************************************
bool ModuleBase_WidgetSelector::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  GeomShapePtr aShape = myWorkshop->selection()->getShape(thePrs);
  ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
  bool aValid = aResult.get();
  if (!isWholeResultAllowed())
    aValid = acceptSubShape(aShape, aResult);

  if (aValid) {
    // In order to avoid selection of the same object
    aResult = myWorkshop->selection()->getResult(thePrs);
    FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aResult);
    aValid = aSelectedFeature != myFeature;
  }
  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetSelector::setSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  ObjectPtr anObject;
  GeomShapePtr aShape;
  getGeomSelection(thePrs, anObject, aShape);

  // the last flag is to be depending on hasObject is called before. To be corrected later
  return ModuleBase_Tools::setObject(attribute(), anObject, aShape,
                                     myWorkshop, myIsInValidate, true);
}

//********************************************************************
void ModuleBase_WidgetSelector::deactivate()
{
  ModuleBase_WidgetValidated::deactivate();
  /// clear temporary cash
  AttributePtr anAttribute = attribute();
  if (!anAttribute.get())
    return;
  std::string aType = anAttribute->attributeType();
  if (anAttribute->attributeType() == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectAttr =
                             std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(anAttribute);
    aSelectAttr->removeTemporaryValues();
  }
  else if (anAttribute->attributeType() == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectAttr =
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(anAttribute);
    aSelectAttr->removeTemporaryValues();
  }
}

//********************************************************************
bool ModuleBase_WidgetSelector::isWholeResultAllowed() const
{
  AttributePtr anAttribute = attribute();
  if (anAttribute.get()) {
    AttributeSelectionListPtr aSelAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(anAttribute);
    if (aSelAttr.get())
      return aSelAttr->isWholeResultAllowed();
  }
  return false;
}

bool getObjectFromGroup(ObjectPtr& theObject, GeomShapePtr& theShape)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);

  AttributeSelectionListPtr anAttrList = aFeature->selectionList("group_list");

  for (int anIndex = 0; anIndex < anAttrList->size(); ++anIndex) {
    AttributeSelectionPtr aSelect = anAttrList->value(anIndex);
    if (aSelect->context()->shape()->isSubShape(theShape) ||
        aSelect->context()->shape()->isEqual(theShape)) {
      theObject = aSelect->contextObject();
      return true;
    }
  }
  return false;
}
