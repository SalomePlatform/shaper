// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_Validators.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Circle.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintDistance.h"
#include "SketchPlugin_ConstraintFillet.h"
#include "SketchPlugin_ConstraintRigid.h"
#include "SketchPlugin_ConstraintTangent.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Tools.h"

#include "SketcherPrs_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Lin.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Vertex.h>
#include <GeomDataAPI_Point2D.h>

#include <algorithm>
#include <cmath>

const double tolerance = 1.e-7;

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
      dynamic_cast<const ModelAPI_AttributeValidator*>(aFactory->validator("GeomValidators_ShapeType"));
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


static bool hasCoincidentPoint(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  FeaturePtr aConstrFeature;
  std::list<AttributePtr> anAttrList;
  if (theFeature1->getKind() == SketchPlugin_Circle::ID() ||
      theFeature2->getKind() == SketchPlugin_Circle::ID())
    return false;
  if (theFeature2->getKind() == SketchPlugin_Line::ID()) {
    anAttrList.push_back(theFeature2->attribute(SketchPlugin_Line::START_ID()));
    anAttrList.push_back(theFeature2->attribute(SketchPlugin_Line::END_ID()));
  } else if (theFeature2->getKind() == SketchPlugin_Arc::ID()) {
    anAttrList.push_back(theFeature2->attribute(SketchPlugin_Arc::START_ID()));
    anAttrList.push_back(theFeature2->attribute(SketchPlugin_Arc::END_ID()));
  }

  const std::set<AttributePtr>& aRefsList = theFeature1->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aRefIt = aRefsList.begin();
  for (; aRefIt != aRefsList.end(); ++aRefIt) {
    aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIt)->owner());
    if (aConstrFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
      continue;
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefIt);
    AttributePtr anAttr = aRefAttr->attr();
    if (anAttr->id() == SketchPlugin_Arc::CENTER_ID())
      continue;

    anAttr = aConstrFeature->attribute(SketchPlugin_Constraint::ENTITY_A());
    if (anAttr == *aRefIt)
      anAttr = aConstrFeature->attribute(SketchPlugin_Constraint::ENTITY_B());

    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttr);
    if (!aRefAttr)
      continue;
    anAttr = aRefAttr->attr();
    for (std::list<AttributePtr>::const_iterator anIt = anAttrList.begin();
         anIt != anAttrList.end(); ++anIt)
      if (*anIt == anAttr)
        return true;
  }
  return false;
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
    if (!aOtherFea)
      return true;

    if ((aRefFea->getKind() == SketchPlugin_Arc::ID() ||
        aOtherFea->getKind() == SketchPlugin_Arc::ID()) &&
        !hasCoincidentPoint(aRefFea, aOtherFea))
      return false;

    if (aRefFea->getKind() == SketchPlugin_Line::ID()) {
      if (aOtherFea->getKind() != SketchPlugin_Arc::ID() &&
          aOtherFea->getKind() != SketchPlugin_Circle::ID()) {
        theError = "It refers to a " + SketchPlugin_Line::ID() + ", but " + aParamA + " is neither an "
          + SketchPlugin_Arc::ID() + " nor " + SketchPlugin_Circle::ID();
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
    else if (aRefFea->getKind() == SketchPlugin_Circle::ID()) {
      if (aOtherFea->getKind() != SketchPlugin_Line::ID()) {
        theError = "It refers to an " + SketchPlugin_Circle::ID() + ", but " + aParamA + " is not a "
          + SketchPlugin_Line::ID();
        return false;
      }
    }
    else {
      theError = "It refers to " + aRefFea->getKind() + ", but should refer to " + SketchPlugin_Line::ID()
        + " or " + SketchPlugin_Arc::ID() + " or " + SketchPlugin_Circle::ID();
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

  std::string aType[2];
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

    aType[i] = aFeature->getKind();
    if (aFeature->getKind() != SketchPlugin_Line::ID() &&
        aFeature->getKind() != SketchPlugin_Circle::ID() &&
        aFeature->getKind() != SketchPlugin_Arc::ID()) {
      theError = "The " + aFeature->getKind() + " feature kind of attribute is wrong. It should be " +
                 SketchPlugin_Line::ID() + " or " + SketchPlugin_Circle::ID() + " or " + 
                 SketchPlugin_Arc::ID();
      // wrong type of attribute
      return false;
    }
  }

  if ((aType[0] == SketchPlugin_Line::ID() || aType[1] == SketchPlugin_Line::ID()) &&
      aType[0] != aType[1]) {
    theError = "Feature with kinds " + aType[0] + " and " + aType[1] + "can not be equal.";
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

static bool hasSameTangentFeature(const std::set<AttributePtr>& theRefsList, const FeaturePtr theFeature)
{
  for(std::set<AttributePtr>::const_iterator anIt = theRefsList.cbegin(); anIt != theRefsList.cend(); ++anIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*anIt);
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aFeature->getKind() == SketchPlugin_ConstraintTangent::ID()) {
      AttributeRefAttrPtr anAttrRefA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aFeature->attribute(SketchPlugin_ConstraintTangent::ENTITY_A()));
      AttributeRefAttrPtr anAttrRefB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aFeature->attribute(SketchPlugin_ConstraintTangent::ENTITY_B()));
      if(anAttrRefA.get()) {
        ResultPtr aResA = std::dynamic_pointer_cast<ModelAPI_Result>(anAttrRefA->object());
        if(aResA.get()) {
          DocumentPtr aDoc = aResA->document();
          if(aDoc.get()) {
            FeaturePtr aFeatureA = aDoc->feature(aResA);
            if(aFeatureA.get() && aFeatureA == theFeature) {
              return true;
            }
          }
        }
      }
      if(anAttrRefB.get()) {
        ResultPtr aResB = std::dynamic_pointer_cast<ModelAPI_Result>(anAttrRefB->object());
        if(aResB.get()) {
          DocumentPtr aDoc = aResB->document();
          if(aDoc.get()) {
            FeaturePtr aFeatureB = aDoc->feature(aResB);
            if(aFeatureB.get() && aFeatureB == theFeature) {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

bool SketchPlugin_FilletVertexValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments,
                                                 std::string& theError) const
{
  std::shared_ptr<SketchPlugin_ConstraintFillet> aFilletFeature = std::dynamic_pointer_cast<SketchPlugin_ConstraintFillet>(theAttribute->owner());
  AttributeRefAttrListPtr aPointsRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttribute);
  if(aPointsRefList->size() == 0) {
    theError = "Error: List of points is empty.";
    return false;
  }

  std::map<AttributePtr, SketchPlugin_ConstraintFillet::FilletFeatures> aPointsFeaturesMap = aFilletFeature->pointsFeaturesMap();
  std::set<AttributePtr> aSetOfPointsOnResultEdges;
  for(std::map<AttributePtr, SketchPlugin_ConstraintFillet::FilletFeatures>::iterator aPointsIter = aPointsFeaturesMap.begin();
      aPointsIter != aPointsFeaturesMap.end();
      ++aPointsIter) {
    const SketchPlugin_ConstraintFillet::FilletFeatures& aFeatures = aPointsIter->second;
    const std::list<FeaturePtr>& aResultEdges = aFeatures.resultEdges;
    for(std::list<FeaturePtr>::const_iterator aResultIter = aResultEdges.cbegin();
        aResultIter != aResultEdges.cend();
        ++aResultIter) {
      FeaturePtr aResultFeature = *aResultIter;
      if(aResultFeature->getKind() == SketchPlugin_Line::ID()) {
        aSetOfPointsOnResultEdges.insert(aResultFeature->attribute(SketchPlugin_Line::START_ID()));
        aSetOfPointsOnResultEdges.insert(aResultFeature->attribute(SketchPlugin_Line::END_ID()));
      } else if(aResultFeature->getKind() == SketchPlugin_Arc::ID()) {
        aSetOfPointsOnResultEdges.insert(aResultFeature->attribute(SketchPlugin_Arc::START_ID()));
        aSetOfPointsOnResultEdges.insert(aResultFeature->attribute(SketchPlugin_Arc::END_ID()));
      }
    }
  }

  std::list<std::pair<ObjectPtr, AttributePtr>> aPointsList = aPointsRefList->list();
  for(std::list<std::pair<ObjectPtr, AttributePtr>>::const_iterator aPointsIt = aPointsList.cbegin(); aPointsIt != aPointsList.cend(); aPointsIt++) {
    ObjectPtr anObject = (*aPointsIt).first;
    AttributePtr aPointAttribute = (*aPointsIt).second;
    std::shared_ptr<GeomAPI_Pnt2d> aSelectedPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aPointAttribute)->pnt();

    // If we alredy have some result then:
    // - if it is the same point all ok, just skip it
    // - if it is point on the fillet result edge then it is not valid
    if(!aPointsFeaturesMap.empty()) {
      if(aPointsFeaturesMap.find(aPointAttribute) != aPointsFeaturesMap.end()) {
        continue;
      }

      // Check that selected point not on the one of the fillet result edge.
      if(aSetOfPointsOnResultEdges.find(aPointAttribute) != aSetOfPointsOnResultEdges.end()) {
        return false;
      }
    }

    // Obtain constraint coincidence for the fillet point.
    const std::set<AttributePtr>& aRefsList = aPointAttribute->owner()->data()->refsToMe();
    FeaturePtr aConstraintCoincidence;
    for(std::set<AttributePtr>::const_iterator anIt = aRefsList.cbegin(); anIt != aRefsList.cend(); ++anIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*anIt);
      FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
        AttributeRefAttrPtr anAttrRefA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_A()));
        AttributeRefAttrPtr anAttrRefB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_B()));
        if(anAttrRefA.get() && !anAttrRefA->isObject()) {
          AttributePtr anAttrA = anAttrRefA->attr();
          if(aPointAttribute == anAttrA) {
            aConstraintCoincidence = aConstrFeature;
            break;
          }
        }
        if(anAttrRefB.get() && !anAttrRefB->isObject()) {
          AttributePtr anAttrB = anAttrRefB->attr();
          if(aPointAttribute == anAttrB) {
            aConstraintCoincidence = aConstrFeature;
            break;
          }
        }
      }
    }

    if(!aConstraintCoincidence.get()) {
      theError = "Error: one of the selected point does not have coicidence.";
      return false;
    }

    // Get coincides from constraint.
    std::set<FeaturePtr> aCoinsides;
    SketchPlugin_Tools::findCoincidences(aConstraintCoincidence,
                                         SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                         aCoinsides);
    SketchPlugin_Tools::findCoincidences(aConstraintCoincidence,
                                         SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                         aCoinsides);

    // Remove points from set of coincides.
    std::set<FeaturePtr> aNewSetOfCoincides;
    for(std::set<FeaturePtr>::iterator anIt = aCoinsides.begin(); anIt != aCoinsides.end(); ++anIt) {
      if((*anIt)->getKind() != SketchPlugin_Line::ID() &&
         (*anIt)->getKind() != SketchPlugin_Arc::ID()) {
           continue;
      }
      if((*anIt)->getKind() == SketchPlugin_Arc::ID()) {
        AttributePtr anArcCenter = (*anIt)->attribute(SketchPlugin_Arc::CENTER_ID());
        std::shared_ptr<GeomAPI_Pnt2d> anArcCenterPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anArcCenter)->pnt();
        double aDistSelectedArcCenter = aSelectedPnt->distance(anArcCenterPnt);
        if(aDistSelectedArcCenter < tolerance) {
          continue;
        }
      }
      aNewSetOfCoincides.insert(*anIt);
    }
    aCoinsides = aNewSetOfCoincides;

    // If we still have more than two coincides remove auxilary entities from set of coincides.
    if(aCoinsides.size() > 2) {
      aNewSetOfCoincides.clear();
      for(std::set<FeaturePtr>::iterator anIt = aCoinsides.begin(); anIt != aCoinsides.end(); ++anIt) {
        if(!(*anIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value()) {
          aNewSetOfCoincides.insert(*anIt);
        }
      }
      aCoinsides = aNewSetOfCoincides;
    }

    if(aCoinsides.size() != 2) {
      theError = "Error: One of the selected points does not have two suitable edges for fillet.";
      return false;
    }

    // Check that selected edges don't have tangent constraint.
    std::set<FeaturePtr>::iterator anIt = aCoinsides.begin();
    FeaturePtr aFirstFeature = *anIt++;
    FeaturePtr aSecondFeature = *anIt;
    const std::set<AttributePtr>& aFirstFeatureRefsList = aFirstFeature->data()->refsToMe();
    if(hasSameTangentFeature(aFirstFeatureRefsList, aSecondFeature)) {
      theError = "Error: Edges in selected point has tangent constraint.";
      return false;
    }

    std::list<ResultPtr> aFirstResults = aFirstFeature->results();
    for(std::list<ResultPtr>::iterator aResIt = aFirstResults.begin(); aResIt != aFirstResults.end(); ++aResIt) {
      ResultPtr aRes = *aResIt;
      const std::set<AttributePtr>& aResRefsList = aRes->data()->refsToMe();
      if(hasSameTangentFeature(aResRefsList, aSecondFeature)) {
        theError = "Error: Edges in selected point has tangent constraint.";
        return false;
      }
    }

    // Check that lines not collinear
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
  }

  return true;
}

bool SketchPlugin_MiddlePointAttrValidator::isValid(const AttributePtr& theAttribute, 
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
  AttributeRefAttrPtr anOtherAttr = anAttributeFeature->data()->refattr(aParamA);

  AttributeRefAttrPtr aRefAttrs[2] = {aRefAttr, anOtherAttr};
  int aNbPoints = 0;
  int aNbLines = 0;
  for (int i = 0; i < 2; ++i) {
    if (!aRefAttrs[i]->isObject())
      ++aNbPoints;
    else {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttrs[i]->object());
      if (!aFeature) {
        if (aNbPoints + aNbLines != 0)
          return true;
        else continue;
      }

      if (aFeature->getKind() == SketchPlugin_Point::ID())
        ++aNbPoints;
      else if (aFeature->getKind() == SketchPlugin_Line::ID())
        ++aNbLines;
    }
  }

  if (aNbPoints != 1 || aNbLines != 1) {
    theError = "Middle point constraint allows points and lines only";
    return false;
  }
  return true;
}

bool SketchPlugin_ArcTangentPointValidator::isValid(const AttributePtr& theAttribute,
                                                    const std::list<std::string>& /*theArguments*/,
                                                    std::string& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  AttributePtr anAttr = aRefAttr->attr();
  if (!anAttr) {
    theError = "The attribute " + theAttribute->id() + " should be a point";
    return false;
  }

  FeaturePtr anAttrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->owner());
  const std::string& aFeatureType = anAttrFeature->getKind();
  if (aFeatureType == SketchPlugin_Arc::ID()) {
    // selected point should not be a center of arc
    const std::string& aPntId = anAttr->id();
    if (aPntId != SketchPlugin_Arc::START_ID() && aPntId != SketchPlugin_Arc::END_ID()) {
      theError = "The attribute " + aPntId + " is not supported";
      return false;
    }
  }
  else if (aFeatureType == SketchPlugin_Line::ID()) {
    // selected point should be bound point of line
    const std::string& aPntId = anAttr->id();
    if (aPntId != SketchPlugin_Line::START_ID() && aPntId != SketchPlugin_Line::END_ID()) {
      theError = "The attribute " + aPntId + " is not supported";
      return false;
    }
  }
  else {
    theError = "Unable to build tangent arc on " + anAttrFeature->getKind();
    return false;
  }

  return true;
}

bool SketchPlugin_IntersectionValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments,
                                                 std::string& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeSelection::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }
  AttributeSelectionPtr aLineAttr =
                              std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  std::shared_ptr<GeomAPI_Edge> anEdge;
  if(aLineAttr && aLineAttr->value() && aLineAttr->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aLineAttr->value()));
  } else if(aLineAttr->context() && aLineAttr->context()->shape() && aLineAttr->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aLineAttr->context()->shape()));
  }

  if (!anEdge || !anEdge->isLine()) {
    theError = "The attribute " + theAttribute->id() + " should be a line";
    return false;
  }

  std::shared_ptr<GeomAPI_Dir> aLineDir = anEdge->line()->direction();

  // find a sketch
  std::shared_ptr<SketchPlugin_Sketch> aSketch;
  std::set<AttributePtr> aRefs = aLineAttr->owner()->data()->refsToMe();
  std::set<AttributePtr>::const_iterator anIt = aRefs.begin();
  for (; anIt != aRefs.end(); ++anIt) {
    CompositeFeaturePtr aComp =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>((*anIt)->owner());
    if (aComp && aComp->getKind() == SketchPlugin_Sketch::ID()) {
      aSketch = std::dynamic_pointer_cast<SketchPlugin_Sketch>(aComp);
      break;
    }
  }
  if (!aSketch) {
    theError = "There is no sketch referring to the current feature";
    return false;
  }

  std::shared_ptr<GeomAPI_Pln> aPlane = aSketch->plane();
  std::shared_ptr<GeomAPI_Dir> aNormal = aPlane->direction();
  return fabs(aNormal->dot(aLineDir)) > tolerance * tolerance;
}
