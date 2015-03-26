// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Validators.cpp
// Created:     09 July 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_Validators.h"

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomValidators_Tools.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_WidgetShapeSelector.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>

#include <SketchPlugin_Sketch.h>

#include <list>
#ifdef _DEBUG
#include <iostream>
#endif

int shapesNbPoints(const ModuleBase_ISelection* theSelection)
{
  QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected();  
  int aCount = 0;
  foreach (ModuleBase_ViewerPrs aPrs, aList) {
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_VERTEX)
        aCount++;
    }
  }
  return aCount;
}

int shapesNbLines(const ModuleBase_ISelection* theSelection)
{
  QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected();
  int aCount = 0;
  foreach(ModuleBase_ViewerPrs aPrs, aList) {
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_EDGE) {
        TopoDS_Edge aEdge = TopoDS::Edge(aShape);
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

bool PartSet_DistanceValidator::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbPoints(theSelection) + shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_LengthValidator::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 2);
}

bool PartSet_PerpendicularValidator::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_ParallelValidator::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_RadiusValidator::isValid(const ModuleBase_ISelection* theSelection) const
{
  QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected();
  ModuleBase_ViewerPrs aPrs;
  int aCount = 0;
  foreach (ModuleBase_ViewerPrs aPrs, aList) {
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_EDGE) {
        TopoDS_Edge aEdge = TopoDS::Edge(aShape);
        Standard_Real aStart, aEnd;
        Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge, aStart, aEnd);
        GeomAdaptor_Curve aAdaptor(aCurve);
        if (aAdaptor.GetType() == GeomAbs_Circle)
          aCount++;
      }
    }
  }
  return (aCount > 0) && (aCount < 2);
}

bool PartSet_RigidValidator::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 2);
}

bool PartSet_DifferentObjectsValidator::isValid(const AttributePtr& theAttribute, 
                                                const std::list<std::string>& theArguments) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());

  // 1. check whether the object of the attribute is not among the feature attributes
  // find the attribute's object
  ObjectPtr anObject =  GeomValidators_Tools::getObject(theAttribute);

  // check whether the object is not among other feature attributes
  if (anObject.get() != NULL) {
    // Check RefAttr attributes
    std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs = aFeature->data()->attributes("");
    //if (anAttrs.size() > 0) {
    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anIt = anAttrs.begin();
    for(; anIt != anAttrs.end(); anIt++) {
      AttributePtr anAttr = *anIt;
      // the function parameter attribute should be skipped
      if (anAttr.get() == NULL || anAttr->id() == theAttribute->id())
        continue;
      ObjectPtr aCurObject =  GeomValidators_Tools::getObject(anAttr);
      if (aCurObject  && aCurObject == anObject)
        return false;
    }
  }
  else {
    // 2. collect object referenced by theAttribute and ...
    if (featureHasReferences(theAttribute)) {
      // 3. check whether the attribute value is not among other feature attributes
      std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs = 
        aFeature->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
      std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
      for(; anAttr != anAttrs.end(); anAttr++) {
        if (*anAttr) {
          std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
            std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
          // check the object is already presented
          if (!aRef->isObject() && aRef->attr() == theAttribute)
            return false;
        }
// TODO(nds) v1.0.2 master
//bool PartSet_DifferentObjectsValidator::isValid(const FeaturePtr& theFeature,
//                                                const std::list<std::string>& theArguments,
//                                                const ObjectPtr& theObject,
//                                                const GeomShapePtr& theShape) const
//{
//  // Check RefAttr attributes
//  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs =
//    theFeature->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
//  if (anAttrs.size() > 0) {
//    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
//    for(; anAttr != anAttrs.end(); anAttr++) {
//      if (*anAttr) {
//        std::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
//          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
//        // check the object is already presented
//        if (aRef->isObject() && aRef->object() == theObject)
//          return false;
//      }
//    }
//  }
//  // Check selection attributes
//  anAttrs = theFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());
//  if (anAttrs.size() > 0) {
//    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
//    for(; anAttr != anAttrs.end(); anAttr++) {
//      if (*anAttr) {
//        std::shared_ptr<ModelAPI_AttributeSelection> aRef =
//          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*anAttr);
//        // check the object is already presented
//        if (aRef->isInitialized() && aRef->context() == theObject) {
//          if (theShape.get() != NULL) {
//            if (aRef->value()->isEqual(theShape))
//              return false;
//          } else
//            return false;
//        }
//      }
//    }
//  }
//  // Check selection attributes
//  anAttrs = theFeature->data()->attributes(ModelAPI_AttributeReference::typeId());
//  if (anAttrs.size() > 0) {
//    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
//    for(; anAttr != anAttrs.end(); anAttr++) {
//      if (*anAttr) {
//        std::shared_ptr<ModelAPI_AttributeReference> aRef =
//          std::dynamic_pointer_cast<ModelAPI_AttributeReference>(*anAttr);
//        // check the object is already presented
//        if (aRef->isInitialized() && aRef->value() == theObject)
//          return false;
// ======= end of todo
      }
      return true;
    }
  }
  return true;
}

bool PartSet_DifferentObjectsValidator::featureHasReferences(const AttributePtr& theAttribute) const
// TODO(nds) v1.0.2 master
//bool PartSet_DifferentObjectsValidator::isValid(const FeaturePtr& theFeature,
//                                                const std::list<std::string>& theArguments,
//                                                const AttributePtr& theAttribute) const
//{
//  if (PartSet_DifferentObjectsValidator::isValid(theAttribute, theArguments)) {
//    std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs =
//      theFeature->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
//    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
//    for(; anAttr != anAttrs.end(); anAttr++) {
//      if (*anAttr) {
//        std::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
//          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
//        // check the object is already presented
//        if (!aRef->isObject() && aRef->attr() == theAttribute)
//          return false;
//      }
//    }
//    return true;
//  }
//  return false;
//}
//
//bool PartSet_DifferentObjectsValidator::isValid(const AttributePtr& theAttribute,
//                                                const std::list<std::string>& theArguments) const
// ======= end of todo
{
  std::list<std::pair<std::string, std::list<ObjectPtr> > > allRefs;
  if (theAttribute->owner().get() && theAttribute->owner()->data().get())
    theAttribute->owner()->data()->referencesToObjects(allRefs);
  // collect object referenced by theAttribute
  std::list<ObjectPtr>* anAttrObjs = 0;
  std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRefIter = allRefs.begin();
  for(; aRefIter != allRefs.end(); aRefIter++) {
    if (theAttribute->id() == aRefIter->first)
      anAttrObjs = &(aRefIter->second);
  }
  if (!anAttrObjs || anAttrObjs->empty())
    return true; // theAttribute does not references to anything
  // check with all others
  for(aRefIter = allRefs.begin(); aRefIter != allRefs.end(); aRefIter++) {
    if (theAttribute->id() == aRefIter->first)
      continue; // do not check with myself
    std::list<ObjectPtr>::iterator aReferenced = aRefIter->second.begin();
    for(; aReferenced != aRefIter->second.end(); aReferenced++) {
      std::list<ObjectPtr>::iterator aReferencedByMe = anAttrObjs->begin();
      for(; aReferencedByMe != anAttrObjs->end(); aReferencedByMe++) {
        if (*aReferenced == *aReferencedByMe) // found same objects!
          return false;
      }
    }
  }
  return true;
}

bool PartSet_SketchEntityValidator::isValid(const AttributePtr& theAttribute,
                                            const std::list<std::string>& theArguments) const
{
  bool isSketchEntities = true;
  std::set<std::string> anEntityKinds;
  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();
  for (; anIt != aLast; anIt++) {
    anEntityKinds.insert(*anIt);
  }

  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    // it filters only selection list attributes
    std::string aType = aSelectionListAttr->selectionType().c_str();
    // all context objects should be sketch entities
    int aSize = aSelectionListAttr->size();
    for (int i = 0; i < aSelectionListAttr->size() && isSketchEntities; i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
      ObjectPtr anObject = aSelectAttr->context();
      FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
      isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
    }
  }
  if (anAttributeType == ModelAPI_AttributeRefAttr::typeId()) {
    std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
                     std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    isSketchEntities = false;
    if (aRef->isObject()) {
      ObjectPtr anObject = aRef->object();
      if (anObject.get() != NULL) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
        if (aFeature.get() != NULL)
          isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
      }
    }
  }

  return isSketchEntities;
}



bool PartSet_SameTypeAttrValidator::isValid(
  const AttributePtr& theAttribute, const std::list<std::string>& theArguments ) const
{
  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (!aRefAttr)
    return false;

  bool isObject = aRefAttr->isObject();
  ObjectPtr anObject = aRefAttr->object();
  if (isObject && anObject) {
    FeaturePtr aRefFea = ModelAPI_Feature::feature(anObject);

    AttributeRefAttrPtr aOtherAttr = aFeature->data()->refattr(aParamA);
    ObjectPtr aOtherObject = aOtherAttr->object();
    FeaturePtr aOtherFea = ModelAPI_Feature::feature(aOtherObject);
    return aRefFea->getKind() == aOtherFea->getKind();
  }
  return false;
}

