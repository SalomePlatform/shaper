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
#include <ModuleBase_ISelection.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Object.h>

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

bool PartSet_DifferentObjectsValidator::isValid(const FeaturePtr& theFeature, 
                                                const std::list<std::string>& theArguments,
                                                const ObjectPtr& theObject) const
{
  // Check RefAttr attributes
  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs = 
    theFeature->data()->attributes(ModelAPI_AttributeRefAttr::type());
  if (anAttrs.size() > 0) {
    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
      if (*anAttr) {
        std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
        // check the object is already presented
        if (aRef->isObject() && aRef->object() == theObject)
          return false;
      }
    }
  }
  // Check selection attributes
  anAttrs = theFeature->data()->attributes(ModelAPI_AttributeSelection::type());
  if (anAttrs.size() > 0) {
    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
      if (*anAttr) {
        std::shared_ptr<ModelAPI_AttributeSelection> aRef = 
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*anAttr);
        // check the object is already presented
        if (aRef->isInitialized() && aRef->context() == theObject)
          return false;
      }
    }
  }
  // Check selection attributes
  anAttrs = theFeature->data()->attributes(ModelAPI_AttributeReference::type());
  if (anAttrs.size() > 0) {
    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
      if (*anAttr) {
        std::shared_ptr<ModelAPI_AttributeReference> aRef = 
          std::dynamic_pointer_cast<ModelAPI_AttributeReference>(*anAttr);
        // check the object is already presented
        if (aRef->isInitialized() && aRef->value() == theObject)
          return false;
      }
    }
  }
  return true;
}

bool PartSet_DifferentObjectsValidator::isValid(const FeaturePtr& theFeature, 
                                                const std::list<std::string>& theArguments,
                                                const AttributePtr& theAttribute) const
{
  // not implemented
  return true;
}

bool PartSet_DifferentObjectsValidator::isValid(const AttributePtr& theAttribute, 
                                                const std::list<std::string>& theArguments) const
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

bool PartSet_SketchValidator::isValid(const ObjectPtr theObject) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  return aFeature->getKind() == SketchPlugin_Sketch::ID();
}
