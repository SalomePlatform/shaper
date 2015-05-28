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
#include <GeomAPI_Edge.h>

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

bool PartSet_DistanceSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbPoints(theSelection) + shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_LengthSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount == 1);
}

bool PartSet_PerpendicularSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_ParallelSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_RadiusSelection::isValid(const ModuleBase_ISelection* theSelection) const
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
  return (aCount == 1);
}

bool PartSet_RigidSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected();  
  return (aList.count() == 1);
}


bool PartSet_CoincidentSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbPoints(theSelection);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_HVDirSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount == 1);
}

bool PartSet_FilletSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  int aCount = shapesNbLines(theSelection);
  return (aCount > 0) && (aCount < 3);
}

bool PartSet_TangentSelection::isValid(const ModuleBase_ISelection* theSelection) const
{
  QList<ModuleBase_ViewerPrs> aList = theSelection->getSelected();
  if ((aList.size() == 0) || (aList.size() > 2))
    return false;

  ModuleBase_ViewerPrs aPrs = aList.first();
  const TopoDS_Shape& aShape = aPrs.shape();
  if (aShape.IsNull())
    return false;

  if (aShape.ShapeType() != TopAbs_EDGE)
    return false;

  std::shared_ptr<GeomAPI_Shape> aShapePtr(new GeomAPI_Shape);
  aShapePtr->setImpl(new TopoDS_Shape(aShape));
  GeomAPI_Edge aEdge1(aShapePtr);

  if (aEdge1.isLine() || aEdge1.isArc()) {
    if (aList.size() == 2) {
      // Check second selection
      aPrs = aList.last();
      const TopoDS_Shape& aShape2 = aPrs.shape();
      if (aShape2.IsNull())
        return false;

      if (aShape2.ShapeType() != TopAbs_EDGE)
        return false;

      std::shared_ptr<GeomAPI_Shape> aShapePtr2(new GeomAPI_Shape);
      aShapePtr2->setImpl(new TopoDS_Shape(aShape2));
      GeomAPI_Edge aEdge2(aShapePtr2);
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
            for(int j = 0; j < aRefSelList->size(); j++) {
              std::shared_ptr<ModelAPI_AttributeSelection> aRefSel = aRefSelList->value(j);
              if(aCurSel->context() == aRefSel->context()) {
                if(aCurSel->value().get() == NULL || aRefSel->value().get() == NULL
                  || aCurSel->value()->isEqual(aRefSel->value())) {
                    return false;
                }
              }
            }
          }
        }
      }
    }
  }
  return !featureHasReferences(theAttribute);
}

bool PartSet_DifferentObjectsValidator::featureHasReferences(const AttributePtr& theAttribute) const
{
  std::list<std::pair<std::string, std::list<ObjectPtr> > > allRefs;
  if (theAttribute->owner().get() && theAttribute->owner()->data()->isValid())
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
      // a context of the selection attribute is a feature result. It can be a case when the result
      // of the feature is null, e.g. the feature is modified and has not been executed yet.
      // The validator returns an invalid result here. The case is an extrusion built on a sketch
      // feature. A new sketch element creation leads to an empty result.
      if (!anObject.get())
        isSketchEntities = false;
      else {
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
        isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
      }
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

