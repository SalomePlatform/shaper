// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Validators.cpp
// Created:     09 July 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_Validators.h"

#include "PartSet_Tools.h"
#include "PartSet_SketcherMgr.h"

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_OperationFeature.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Arc.h>
#include <GeomAPI_Edge.h>

#include <list>
#ifdef _DEBUG
#include <iostream>
#endif

int shapesNbPoints(const ModuleBase_ISelection* theSelection)
{
  QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected(ModuleBase_ISelection::Viewer);
  ModuleBase_ISelection::filterSelectionOnEqualPoints(aList);

  int aCount = 0;
  foreach (ModuleBase_ViewerPrs aPrs, aList) {
    const GeomShapePtr& aShape = aPrs.shape();
    if (aShape.get() && !aShape->isNull()) {
      if (aShape->shapeType() == GeomAPI_Shape::VERTEX)
        aCount++;
    }
  }
  return aCount;
}

int shapesNbLines(const ModuleBase_ISelection* theSelection)
{
  QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected(ModuleBase_ISelection::Viewer);
  int aCount = 0;
  foreach(ModuleBase_ViewerPrs aPrs, aList) {
    const GeomShapePtr& aShape = aPrs.shape();
    if (aShape.get() && !aShape->isNull()) {
      if (aShape->shapeType() == GeomAPI_Shape::EDGE) {
        const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
        TopoDS_Edge aEdge = TopoDS::Edge(aTDShape);
        Standard_Real aStart, aEnd;
        Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge, aStart, aEnd);
        GeomAdaptor_Curve aAdaptor(aCurve);
        if (aAdaptor.GetType() == GeomAbs_Line)
          aCount++;
      }
    }
  }
  return aCount;
}


std::shared_ptr<GeomAPI_Pln> sketcherPlane(ModuleBase_Operation* theOperation)
{
  std::shared_ptr<GeomAPI_Pln> aEmptyPln;
  if (theOperation) {
    ModuleBase_OperationFeature* aFeatureOp = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
    if (aFeatureOp) {
      CompositeFeaturePtr aFeature = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeatureOp->feature());
      if (aFeature && (aFeature->getKind() == SketchPlugin_Sketch::ID()))
        return PartSet_Tools::sketchPlane(aFeature);
    }
  }
  return aEmptyPln; 
}


bool isEmptySelectionValid(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFeatureOp = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  // during the create operation empty selection is always valid
  if (!aFeatureOp->isEditOperation()) {
    return true;
  }
  else {
    if (PartSet_SketcherMgr::isSketchOperation(aFeatureOp)) {
      std::shared_ptr<GeomAPI_Pln> aPlane = sketcherPlane(theOperation);
      if (aPlane.get())
        return true;
      else 
        return false;
    }
    else// in edit operation an empty selection is always valid, performed for re-entrant operrations
      return true;
  }
}

bool PartSet_DistanceSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    int aCount = shapesNbPoints(theSelection) + shapesNbLines(theSelection);
    return (aCount > 0) && (aCount < 3);
  }
}

bool PartSet_LengthSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    int aCount = shapesNbLines(theSelection);
    return (aCount == 1);
  }
}

bool PartSet_PerpendicularSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    int aCount = shapesNbLines(theSelection);
    return (aCount > 0) && (aCount < 3);
  }
}

bool PartSet_ParallelSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    int aCount = shapesNbLines(theSelection);
    return (aCount > 0) && (aCount < 3);
  }
}

bool PartSet_RadiusSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected(ModuleBase_ISelection::Viewer);
    ModuleBase_ViewerPrs aPrs;
    int aCount = 0;
    foreach (ModuleBase_ViewerPrs aPrs, aList) {
      const GeomShapePtr& aShape = aPrs.shape();
      if (aShape.get() && !aShape->isNull()) {
        if (aShape->shapeType() == GeomAPI_Shape::EDGE) {
          const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
          TopoDS_Edge aEdge = TopoDS::Edge(aTDShape);
          Standard_Real aStart, aEnd;
          Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge, aStart, aEnd);
          GeomAdaptor_Curve aAdaptor(aCurve);
          if (aAdaptor.GetType() == GeomAbs_Circle)
            aCount++;
        }
      }
    }
    return (aCount == 1);
  }
}

bool PartSet_RigidSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected(ModuleBase_ISelection::Viewer);
    return (aList.count() == 1);
  }
}


bool PartSet_CoincidentSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    // Coincident can be applied to points and to lines
    int aCount = shapesNbPoints(theSelection);
    aCount += shapesNbLines(theSelection);
    return (aCount > 0) && (aCount < 3);
  }
}

bool PartSet_HVDirSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    int aCount = shapesNbLines(theSelection);
    return (aCount == 1);
  }
}

bool PartSet_FilletSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    int aCount = shapesNbPoints(theSelection);
    return aCount == 1;
  }
}

bool PartSet_TangentSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected(ModuleBase_ISelection::Viewer);
    if ((aList.size() == 0) || (aList.size() > 2))
      return false;

    ModuleBase_ViewerPrs aPrs = aList.first();
    const GeomShapePtr& aShape = aPrs.shape();
    if (!aShape.get() || aShape->isNull() || aShape->shapeType() != GeomAPI_Shape::EDGE)
      return false;
    GeomAPI_Edge aEdge1(aShape);

    if (aEdge1.isLine() || aEdge1.isArc()) {
      if (aList.size() == 2) {
        // Check second selection
        aPrs = aList.last();
        const GeomShapePtr& aShape2 = aPrs.shape();
        if (!aShape2.get() || aShape2->isNull() || aShape2->shapeType() != GeomAPI_Shape::EDGE)
          return false;
        GeomAPI_Edge aEdge2(aShape2);

        if (aEdge1.isLine() && aEdge2.isArc())
          return true;
        else if (aEdge1.isArc() && aEdge2.isLine())
          return true;
        else
          return false;
      } else
        return true;
    }
    return false;
  }
}

bool PartSet_AngleSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    int aCount = shapesNbLines(theSelection);
    return (aCount > 0) && (aCount < 3);
  }
}

bool PartSet_EqualSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected(ModuleBase_ISelection::Viewer);
    ModuleBase_ViewerPrs aPrs;
    int aCount = 0;
    int aType = 0;
    foreach (ModuleBase_ViewerPrs aPrs, aList) {
      GeomShapePtr aShape = aPrs.shape();
      if (aShape.get() && aShape->isEdge()) {
        aCount++;
        GeomAPI_Edge aEdge(aShape);
        if (aEdge.isLine()) {
          if (aCount == 1)
            aType = 1;
          else if (aType != 1)
            return false;
        } else if (aEdge.isCircle()) {
          if (aCount == 1)
            aType = 2;
          else if (aType != 2)
            return false;
        } else if (aEdge.isArc()) {
          if (aCount == 1)
            aType = 3;
          else if (aType != 3)
            return false;
        }
      } else
        return false;
    }
    return (aCount > 0) && (aCount < 3);
  }
}

bool PartSet_CollinearSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0) {
    return isEmptySelectionValid(theOperation);
  } else {
    int aCount = shapesNbLines(theSelection);
    return (aCount > 0) && (aCount < 3);
  }
}

bool PartSet_MiddlePointSelection::isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const
{
  if (theSelection->getSelected(ModuleBase_ISelection::Viewer).size() == 0)
    return isEmptySelectionValid(theOperation);
  else
    return shapesNbLines(theSelection) == 1 && shapesNbPoints(theSelection) == 1;
}


std::string PartSet_DifferentObjectsValidator::errorMessage(
                         const PartSet_DifferentObjectsValidator::ErrorType& theType,
                         const std::string& thEqualObject, const std::string& theFirstAttribute,
                         const std::string& theSecondAttribute) const
{
  std::string anError;
  switch (theType) {
    case EqualObjects:
      anError = "The feature uses one " + thEqualObject + " object in " +
                theFirstAttribute + " and " + theSecondAttribute + " attributes.";
      break;
    case EqualAttributes:
      anError = "The feature uses reference to one " + thEqualObject + " attribute in " +
                theFirstAttribute + " and " + theSecondAttribute + " attributes.";
      break;
    case EqualShapes:
      anError = "The feature uses one shape in " +
                theFirstAttribute + " and " + theSecondAttribute + " attributes.";
      break;
    case EmptyShapes:
      anError = "The feature uses empty shapes in " +
                theFirstAttribute + " and " + theSecondAttribute + " attributes.";
      break;
      break;
    default:
      break;
  }
  return anError;
}

bool PartSet_DifferentObjectsValidator::isValid(const AttributePtr& theAttribute, 
                                                const std::list<std::string>& theArguments,
                                                std::string& theError) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());

  // the type of validated attributes should be equal, attributes with different types are not validated
  // Check RefAttr attributes
  std::string anAttrType = theAttribute->attributeType();
  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs;

  if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    bool isObject = anAttr->isObject();
    ObjectPtr anObject = anAttr->object();
    AttributePtr anAttributeAttr = anAttr->attr();

    anAttrs = aFeature->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
    if (anAttrs.size() > 0) {
      std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
      for(; anAttr != anAttrs.end(); anAttr++) {
      if ((*anAttr).get() && (*anAttr)->id() != theAttribute->id()) {
          std::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
                                      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
          if (aRef->isObject() != isObject)
            continue;
          if (isObject) {
            if (aRef->object() == anObject) {
              theError = errorMessage(EqualObjects, anObject.get() ? anObject->data()->name() : "",
                                      theAttribute->id(), aRef->id());
              return false;
            }
          }
          else { // the attribute reference
            if (aRef->attr() == anAttributeAttr) {
              theError = errorMessage(EqualAttributes,
                                      anAttributeAttr.get() ? anAttributeAttr->id() : "",
                                      theAttribute->id(), aRef->id());
              return false;
            }
          }
        }
      }
    }
  }
  else if (anAttrType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttr->context();
    GeomShapePtr aShape = anAttr->value();

    // Check selection attributes
    anAttrs = aFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());
    if (anAttrs.size() > 0) {
      std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
      for(; anAttr != anAttrs.end(); anAttr++) {
        if ((*anAttr).get() && (*anAttr)->id() != theAttribute->id()) {
          std::shared_ptr<ModelAPI_AttributeSelection> aRef =
                                        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*anAttr);
          // check the object is already presented
          if (aRef->context() == aContext) {
            bool aHasShape = aShape.get() != NULL;
            if (!aHasShape || aRef->value()->isEqual(aShape)) {
              theError = errorMessage(EqualShapes, "", theAttribute->id(), aRef->id());
              return false;
            }
          }
        }
      }
    }
  }
  else if (anAttrType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    ObjectPtr anObject = anAttr->value();
    // Check selection attributes
    anAttrs = aFeature->data()->attributes(ModelAPI_AttributeReference::typeId());
    if (anAttrs.size() > 0) {
      std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
      for(; anAttr != anAttrs.end(); anAttr++) {
        if ((*anAttr).get() && (*anAttr)->id() != theAttribute->id()) {
          std::shared_ptr<ModelAPI_AttributeReference> aRef =
            std::dynamic_pointer_cast<ModelAPI_AttributeReference>(*anAttr);
          // check the object is already presented
          if (aRef->value() == anObject) {
            theError = errorMessage(EqualObjects, anObject.get() ? anObject->data()->name() : "",
                                    theAttribute->id(), aRef->id());
            return false;
          }
        }
        return true;
      }
    }
  }
  else if(anAttrType == ModelAPI_AttributeSelectionList::typeId()) {
    std::shared_ptr<ModelAPI_AttributeSelectionList> aCurSelList = 
            std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    anAttrs = aFeature->data()->attributes(ModelAPI_AttributeSelectionList::typeId());
    if(anAttrs.size() > 0) {
      std::list<std::shared_ptr<ModelAPI_Attribute>>::iterator anAttrItr = anAttrs.begin();
      for(; anAttrItr != anAttrs.end(); anAttrItr++){
        if ((*anAttrItr).get() && (*anAttrItr)->id() != theAttribute->id()){
          std::shared_ptr<ModelAPI_AttributeSelectionList> aRefSelList = 
            std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*anAttrItr);
          for(int i = 0; i < aCurSelList->size(); i++) {
            std::shared_ptr<ModelAPI_AttributeSelection> aCurSel = aCurSelList->value(i);
            ResultPtr aCurSelContext = aCurSel->context();
            ResultCompSolidPtr aCurSelCompSolidPtr = ModelAPI_Tools::compSolidOwner(aCurSelContext);
            std::shared_ptr<GeomAPI_Shape> aCurSelCompSolid;
            if(aCurSelCompSolidPtr.get()) {
              aCurSelCompSolid = aCurSelCompSolidPtr->shape();
            }
            for(int j = 0; j < aRefSelList->size(); j++) {
              std::shared_ptr<ModelAPI_AttributeSelection> aRefSel = aRefSelList->value(j);
              ResultPtr aRefSelContext = aRefSel->context();
              ResultCompSolidPtr aRefSelCompSolidPtr = ModelAPI_Tools::compSolidOwner(aRefSelContext);
              std::shared_ptr<GeomAPI_Shape> aRefSelCompSolid;
              if(aRefSelCompSolidPtr.get()) {
                aRefSelCompSolid = aRefSelCompSolidPtr->shape();
              }
              if ((aCurSelCompSolid.get() && aCurSelCompSolid->isEqual(aRefSel->value()))
                || (aRefSelCompSolid.get() && aRefSelCompSolid->isEqual(aCurSel->value()))) {
                  theError = errorMessage(EqualShapes, "", theAttribute->id(),
                                          aRefSel->id());
                  return false;
              }
              if(aCurSelContext == aRefSelContext) {
                if (aCurSel->value().get() == NULL || aRefSel->value().get() == NULL) {
                  theError = errorMessage(EmptyShapes, "", theAttribute->id(),
                                          aRefSel->id());
                  return false;
                }
                if (aCurSel->value()->isEqual(aRefSel->value())) {
                  theError = errorMessage(EqualShapes, "", theAttribute->id(),
                                          aRefSel->id());
                  return false;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (anAttrType == ModelAPI_AttributeRefList::typeId()) {
    std::shared_ptr<ModelAPI_AttributeRefList> aCurSelList =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    anAttrs = aFeature->data()->attributes(ModelAPI_AttributeRefList::typeId());
    if (anAttrs.size() > 0) {
      std::list<std::shared_ptr<ModelAPI_Attribute>>::iterator anAttrItr = anAttrs.begin();
      for (; anAttrItr != anAttrs.end(); anAttrItr++){
        if ((*anAttrItr).get() && (*anAttrItr)->id() != theAttribute->id()){
          std::shared_ptr<ModelAPI_AttributeRefList> aRefSelList =
            std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anAttrItr);
          for (int i = 0; i < aCurSelList->size(); i++) {
            ObjectPtr aCurSelObject = aCurSelList->object(i);
            for (int j = 0; j < aRefSelList->size(); j++) {
              if (aCurSelObject == aRefSelList->object(j)) {
                theError = errorMessage(EqualObjects,
                              aCurSelObject.get() ? aCurSelObject->data()->name() : "",
                              theAttribute->id(), aCurSelList->id());
                return false;
              }
            }
          }
        }
      }
    }
  }
  return true;
}

bool PartSet_CoincidentAttr::isValid(const AttributePtr& theAttribute, 
                                     const std::list<std::string>& theArguments,
                                     std::string& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }

  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  QList<FeaturePtr> aCoinsideLines;
  QList<FeaturePtr> aCoins;

  bool isObject = aRefAttr->isObject();
  ObjectPtr anObject = aRefAttr->object();
  if (isObject && anObject) {
    FeaturePtr aRefFea = ModelAPI_Feature::feature(anObject);
    AttributeRefAttrPtr aOtherAttr = aFeature->data()->refattr(aParamA);
    ObjectPtr aOtherObject = aOtherAttr->object();
    // if the other attribute is not filled still, the result is true
    if (!aOtherObject.get())
      return true;
    FeaturePtr aOtherFea = ModelAPI_Feature::feature(aOtherObject);

    // check that both have coincidence
    FeaturePtr aConstrFeature;
    std::set<FeaturePtr> aCoinList;
    const std::set<std::shared_ptr<ModelAPI_Attribute>>& aRefsList = aRefFea->data()->refsToMe();
    std::set<std::shared_ptr<ModelAPI_Attribute>>::const_iterator aIt;
    for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
        AttributeRefAttrPtr aRAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aAttr);
        AttributePtr aAR = aRAttr->attr();
        if (aAR->id() != SketchPlugin_Arc::CENTER_ID()) // ignore constraint to center of arc
          aCoinList.insert(aConstrFeature);
          PartSet_Tools::findCoincidences(aConstrFeature, aCoinsideLines, aCoins,
                                          SketchPlugin_ConstraintCoincidence::ENTITY_A());
          PartSet_Tools::findCoincidences(aConstrFeature, aCoinsideLines, aCoins,
                                          SketchPlugin_ConstraintCoincidence::ENTITY_B());
      }
    }
    // if there is no coincidence then it is not valid
    if (aCoinList.size() > 0) {
      QList<FeaturePtr>::const_iterator anIt = aCoinsideLines.begin(), aLast = aCoinsideLines.end();
      bool aValid = false;
      for (; anIt != aLast && !aValid; anIt++) {
        aValid = *anIt == aOtherFea;
      }
      if (aValid)
        return true;
    }
  }
  theError = "There is no a common coincident point.";
  return false;
}

