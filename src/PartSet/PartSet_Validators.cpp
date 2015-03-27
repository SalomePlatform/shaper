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
            if (aRef->object() == anObject)
              return false;
          }
          else { // the attribute reference
            if (aRef->attr() == theAttribute)
              return false;
          }
        }
      }
    }
  }
  else if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
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
            if (!aHasShape || aRef->value()->isEqual(aShape))
              return false;
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
          if (aRef->value() == anObject)
            return false;
        }
        return true;
      }
    }
  }
  return !featureHasReferences(theAttribute);
}

bool PartSet_DifferentObjectsValidator::featureHasReferences(const AttributePtr& theAttribute) const
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
    return false; // theAttribute does not references to anything
  // check with all others
  for(aRefIter = allRefs.begin(); aRefIter != allRefs.end(); aRefIter++) {
    if (theAttribute->id() == aRefIter->first)
      continue; // do not check with myself
    std::list<ObjectPtr>::iterator aReferenced = aRefIter->second.begin();
    for(; aReferenced != aRefIter->second.end(); aReferenced++) {
      std::list<ObjectPtr>::iterator aReferencedByMe = anAttrObjs->begin();
      for(; aReferencedByMe != anAttrObjs->end(); aReferencedByMe++) {
        if (*aReferenced == *aReferencedByMe) // found same objects!
          return true;
      }
    }
  }
  return false;
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
