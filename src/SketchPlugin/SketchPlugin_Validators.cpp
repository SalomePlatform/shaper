// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_Validators.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Circle.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintDistance.h"
#include "SketchPlugin_ConstraintRigid.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Tools.h"

#include "SketcherPrs_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>

#include <GeomValidators_ShapeType.h>

#include <GeomDataAPI_Point2D.h>


bool SketchPlugin_DistanceAttrValidator::isValid(const AttributePtr& theAttribute, 
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

  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  bool isObject = aRefAttr->isObject();
  if (!isObject) {
    // an attribute is a point. A point value is valid always for the distance
    return true;
  } else {
    // 1. check whether the references object is a linear
    ObjectPtr anObject = aRefAttr->object();

    const ModelAPI_AttributeValidator* aShapeValidator = 
      dynamic_cast<const GeomValidators_ShapeType*>(aFactory->validator("GeomValidators_ShapeType"));
    std::list<std::string> anArguments;
    anArguments.push_back("circle");
    std::string aCircleError;
    bool aShapeValid = aShapeValidator->isValid(aRefAttr, anArguments, aCircleError);
    // the circle line is not a valid case
    if (aShapeValid) {
      theError = "Circle can not be used in distance constraint";
      return false;
    }
      
    anArguments.clear();
    anArguments.push_back("line");
    std::string aLineError;
    aShapeValid = aShapeValidator->isValid(aRefAttr, anArguments, aLineError);
    // if the attribute value is not a line, that means it is a vertex. A vertex is always valid
    if (aShapeValid) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
      // If it is a line then we have to check that first attribute id not a line
      std::shared_ptr<SketchPlugin_Feature> aSFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(theAttribute->owner());
      SketchPlugin_Sketch* aSketch = aSFeature->sketch();
      std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(aSketch);
      std::shared_ptr<GeomDataAPI_Point2D> aPoint = SketcherPrs_Tools::getFeaturePoint(
        aFeature->data(), aParamA, aPlane);
      if (!aPoint.get()) {
        theError = "One of parameters of distance constraint should be a point";
        return false;
      }
    }
  }
  return true;
}

bool SketchPlugin_TangentAttrValidator::isValid(const AttributePtr& theAttribute, 
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

  FeaturePtr anAttributeFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

  bool isObject = aRefAttr->isObject();
  ObjectPtr anObject = aRefAttr->object();
  if (isObject && anObject.get()) {
    FeaturePtr aRefFea = ModelAPI_Feature::feature(anObject);

    AttributeRefAttrPtr aOtherAttr = anAttributeFeature->data()->refattr(aParamA);
    ObjectPtr aOtherObject = aOtherAttr->object();
    FeaturePtr aOtherFea = ModelAPI_Feature::feature(aOtherObject);

    if (aRefFea->getKind() == SketchPlugin_Line::ID()) {
      if (aOtherFea->getKind() != SketchPlugin_Arc::ID()) {
        theError = "It refers to a " + SketchPlugin_Line::ID() + ", but " + aParamA + " is not an "
          + SketchPlugin_Arc::ID();
        return false;
      }
    }
    else if (aRefFea->getKind() == SketchPlugin_Arc::ID()) {
      if (aOtherFea->getKind() != SketchPlugin_Line::ID() &&
        aOtherFea->getKind() != SketchPlugin_Arc::ID()) {
        theError = "It refers to an " + SketchPlugin_Arc::ID() + ", but " + aParamA + " is not a "
          + SketchPlugin_Line::ID() + " or an " + SketchPlugin_Arc::ID();
        return false;
      }
    }
    else {
      theError = "It refers to " + aRefFea->getKind() + "but should refer to " + SketchPlugin_Line::ID()
        + " or " + SketchPlugin_Arc::ID();
      return false;
    }
    return true;
  }
  else {
    theError = "It uses an empty object";
    return false;
  }

  return true;
}

bool SketchPlugin_NotFixedValidator::isValid(const AttributePtr& theAttribute, 
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }

  std::shared_ptr<SketchPlugin_Feature> aFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theAttribute->owner());
  if (!aFeature)
    return true;

  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

  SketchPlugin_Sketch* aSketch = aFeature->sketch();
  int aNbFeatures = aSketch->numberOfSubs();
  for (int anInd = 0; anInd < aNbFeatures; anInd++) {
    FeaturePtr aSubFeature = aSketch->subFeature(anInd);
    if (aSubFeature->getKind() != SketchPlugin_ConstraintRigid::ID() || aSubFeature == aFeature)
      continue;
    AttributeRefAttrPtr aRAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aSubFeature->attribute(SketchPlugin_ConstraintRigid::ENTITY_A()));
    if (aRefAttr->isObject()) {
      if (aRefAttr->object() == aRAttr->object()) {
        ObjectPtr anObject = aRefAttr->object();
        std::string aName = anObject.get() ? anObject->data()->name() : "";
        theError = "The object " + aName + " has been already fixed.";
        return false;
      }
    }
    else if (aRefAttr->attr() == aRAttr->attr()) {
      AttributePtr anAttribute = aRefAttr->attr();
      std::string aName = anAttribute.get() ? anAttribute->id() : "";
      theError = "The attribute " + aName + " has been already fixed.";
      return false;
    }
  }
  return true;
}

bool SketchPlugin_EqualAttrValidator::isValid(const AttributePtr& theAttribute, 
                                              const std::list<std::string>& theArguments,
                                              std::string& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }

  std::string aParamA = theArguments.front();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttr[2];
  aRefAttr[0] = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  aRefAttr[1] = aFeature->data()->refattr(aParamA);

  if (!aRefAttr[0]->isObject() || !aRefAttr[1]->isObject()) {
    theError = "Attributes can not be used in equal constraint";
    return false;
  }

  int aType[2] = {0, 0}; // types of attributes: 0 - incorrect, 1 - line, 2 - circle, 3 - arc
  std::list<std::string> anArguments;
  for (int i = 0; i < 2; i++) {
    ObjectPtr anObject = aRefAttr[i]->object();
    if (!anObject.get()) {
      theError = "An empty object is used.";
      return false;
    }

    aFeature = ModelAPI_Feature::feature(anObject);
    if (!aFeature.get()) {
      theError = "An empty feature is used.";
      return false;
    }

    if (aFeature->getKind() == SketchPlugin_Line::ID()) {
      aType[i] = 1;
      continue;
    }
    if (aFeature->getKind() == SketchPlugin_Circle::ID()) {
      aType[i] = 2;
      continue;
    }
    if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
      aType[i] = 3;
      continue;
    }
    theError = "The " + aFeature->getKind() + " feature kind of attribute is wrong. It should be " +
               SketchPlugin_Line::ID() + " or " + SketchPlugin_Circle::ID() + " or " + 
               SketchPlugin_Arc::ID();
    // wrong type of attribute
    return false;
  }

  if ((aType[0] == 1 && aType[1] == 2) ||
      (aType[0] == 2 && aType[1] == 1)) {
    theError = "Feature with kinds " + SketchPlugin_Line::ID() + " and " +
               SketchPlugin_Circle::ID() + "can not be equal.";
    return false;
  }
  return true;
}

bool SketchPlugin_MirrorAttrValidator::isValid(const AttributePtr& theAttribute, 
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefList::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefListPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);

  AttributeRefListPtr aRefListOfMirrored = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aFeature->attribute(SketchPlugin_Constraint::ENTITY_C()));
  std::list<ObjectPtr> aMirroredObjects = aRefListOfMirrored->list();

  for(int anInd = 0; anInd < aSelAttr->size(); anInd++) {
    ObjectPtr aSelObject = aSelAttr->object(anInd);
    std::string aName = aSelObject.get() ? aSelObject->data()->name() : "";
    std::list<ObjectPtr>::iterator aMirIter = aMirroredObjects.begin();
    for (; aMirIter != aMirroredObjects.end(); aMirIter++)
      if (aSelObject == *aMirIter) {
        theError = "The object " + aName + " is a result of mirror";
        return false;
      }
  }
  return true;
}


bool SketchPlugin_CoincidenceAttrValidator::isValid(const AttributePtr& theAttribute, 
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

  FeaturePtr aConstraint = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttrA = aConstraint->data()->refattr(aParamA);
  if (!aRefAttrA) {
    theError = "The " + aParamA + " attribute " + " should be " + ModelAPI_AttributeRefAttr::typeId();
    return false;
  }

  AttributeRefAttrPtr aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

  // first attribute is a point, it may coincide with any object
  if (!aRefAttrA->isObject())
    return true;
  else {
    ObjectPtr anObject = aRefAttrA->object();
    if (!anObject.get()) {
      theError = aParamA + " attribute has an empty object";
      return false;
    }
    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttrA->object());
    if (!aFeature.get()) {
      theError = aParamA + " attribute has an empty feature";
      return false;
    }

    if (aFeature->getKind() == SketchPlugin_Point::ID())
      return true;
  }

  // second attribute is a point, it may coincide with any object
  if (!aRefAttrB->isObject())
    return true;
  else {
    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttrB->object());
    if (!aFeature) {
      theError = theAttribute->id() + " attribute has an empty object";
      return false;
    }
    if (aFeature->getKind() == SketchPlugin_Point::ID())
      return true;
  }
  theError = "There is no an attribute filled by a point";
  return false;
}


bool SketchPlugin_CopyValidator::isValid(const AttributePtr& theAttribute, 
                                         const std::list<std::string>& theArguments,
                                         std::string& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefList::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefListPtr aSelAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);

  AttributeRefListPtr aRefListOfInitial = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefListPtr aRefListOfCopied = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aFeature->attribute(SketchPlugin_Constraint::ENTITY_B()));
  std::list<ObjectPtr> anInitialObjects = aRefListOfInitial->list();
  std::list<ObjectPtr> aCopiedObjects = aRefListOfCopied->list();

  std::list<ObjectPtr>::iterator anObjIter;
  for(int anInd = 0; anInd < aSelAttr->size(); anInd++) {
    ObjectPtr aSelObject = aSelAttr->object(anInd);
    anObjIter = anInitialObjects.begin();
    for (; anObjIter != anInitialObjects.end(); anObjIter++)
      if (aSelObject == *anObjIter)
        break;
    if (anObjIter != anInitialObjects.end())
      continue;
    anObjIter = aCopiedObjects.begin();
    for (; anObjIter != aCopiedObjects.end(); anObjIter++)
      if (aSelObject == *anObjIter) {
        std::string aName = aSelObject.get() ? aSelObject->data()->name() : "";
        theError = "The object " + aName + " is a result of copy";
        return false;
      }
  }
  return true;
}

bool SketchPlugin_SolverErrorValidator::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                const std::list<std::string>& theArguments,
                                                std::string& theError) const
{
  AttributeStringPtr aAttributeString = theFeature->string(SketchPlugin_Sketch::SOLVER_ERROR());

  if (!aAttributeString->value().empty()) {
    theError = aAttributeString->value();
    return false;
  }

  return true;
}

bool SketchPlugin_SolverErrorValidator::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  return true;
}

bool SketchPlugin_FilletVertexValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments,
                                                 std::string& theError) const
{
  if(!theAttribute.get()) {
    return false;
  }

  AttributeRefAttrPtr aBase = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if(aBase->isObject()) {
    return false;
  }

  // If we alredy have some result then all ok
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributePtr aBaseLinesAttribute = aFeature->attribute(SketchPlugin_Constraint::ENTITY_C());
  AttributeRefListPtr aRefListOfBaseLines = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(aBaseLinesAttribute);
  if(aRefListOfBaseLines->list().size() == 2) {
    return true;
  }

  AttributePtr anAttrBase = aBase->attr();
  const std::set<AttributePtr>& aRefsList = anAttrBase->owner()->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  FeaturePtr aCoincident;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
      AttributeRefAttrPtr anAttrRefA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_A()));
      AttributeRefAttrPtr anAttrRefB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_B()));
      if(anAttrRefA.get() && !anAttrRefA->isObject()) {
        AttributePtr anAttrA = anAttrRefA->attr();
        if(anAttrBase == anAttrA) {
          aCoincident = aConstrFeature;
          break;
        }
      }
      if(anAttrRefA.get() && !anAttrRefB->isObject()) {
        AttributePtr anAttrB = anAttrRefB->attr();
        if(anAttrBase == anAttrB) {
          aCoincident = aConstrFeature;
          break;
        }
      }
    }
  }

  if(!aCoincident.get()) {
    return false;
  }

  std::set<FeaturePtr> aCoinsideLines;
  SketchPlugin_Tools::findCoincidences(aCoincident,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                       aCoinsideLines);
  SketchPlugin_Tools::findCoincidences(aCoincident,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                       aCoinsideLines);
  if(aCoinsideLines.size() < 2) {
    return false;
  }

  // Remove auxilary lines
  if(aCoinsideLines.size() > 2) {
    std::set<FeaturePtr> aNewLines;
    for(std::set<FeaturePtr>::iterator anIt = aCoinsideLines.begin(); anIt != aCoinsideLines.end(); ++anIt) {
      if(!(*anIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value()) {
        aNewLines.insert(*anIt);
      }
    }
    aCoinsideLines = aNewLines;
  }

  if(aCoinsideLines.size() != 2) {
    return false;
  }

  // Check that lines not collinear
  std::set<FeaturePtr>::iterator anIt = aCoinsideLines.begin();
  FeaturePtr aFirstFeature = *anIt++;
  FeaturePtr aSecondFeature = *anIt;
  if(aFirstFeature->getKind() == SketchPlugin_Line::ID() && aSecondFeature->getKind() == SketchPlugin_Line::ID()) {
    std::string aStartAttr = SketchPlugin_Line::START_ID();
    std::string anEndAttr = SketchPlugin_Line::END_ID();
    std::shared_ptr<GeomAPI_Pnt2d> aFirstStartPnt, aFirstEndPnt, aSecondStartPnt, aSecondEndPnt;
    aFirstStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFirstFeature->attribute(aStartAttr))->pnt();
    aFirstEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFirstFeature->attribute(anEndAttr))->pnt();
    aSecondStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aSecondFeature->attribute(aStartAttr))->pnt();
    aSecondEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aSecondFeature->attribute(anEndAttr))->pnt();
    double aCheck1 = abs((aFirstEndPnt->x() - aFirstStartPnt->x()) * (aSecondStartPnt->y() - aFirstStartPnt->y()) -
      (aSecondStartPnt->x() - aFirstStartPnt->x()) * (aFirstEndPnt->y() - aFirstStartPnt->y()));
    double aCheck2 = abs((aFirstEndPnt->x() - aFirstStartPnt->x()) * (aSecondEndPnt->y() - aFirstStartPnt->y()) -
      (aSecondEndPnt->x() - aFirstStartPnt->x()) * (aFirstEndPnt->y() - aFirstStartPnt->y()));
    if(aCheck1 < 1.e-7 && aCheck2 < 1.e-7) {
      return false;
    }
  }

  return true;
}
