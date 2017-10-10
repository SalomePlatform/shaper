// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "SketchPlugin_Validators.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Circle.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintDistance.h"
#include "SketchPlugin_ConstraintRigid.h"
#include "SketchPlugin_ConstraintTangent.h"
#include "SketchPlugin_Fillet.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_MacroArc.h"
#include "SketchPlugin_MacroCircle.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Trim.h"
#include "SketchPlugin_Tools.h"

#include "SketcherPrs_Tools.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>

#include <ModelGeomAlgo_Point2D.h>
#include <ModelGeomAlgo_Shape.h>

#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Vertex.h>
#include <GeomDataAPI_Point2D.h>

#include <algorithm>
#include <cmath>

const double tolerance = 1.e-7;

bool SketchPlugin_DistanceAttrValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments,
                                                 Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }

  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
                                                                      (theAttribute);
  bool isObject = aRefAttr->isObject();
  if (!isObject) {
    // an attribute is a point. A point value is valid always for the distance
    return true;
  } else {
    // 1. check whether the references object is a linear
    ObjectPtr anObject = aRefAttr->object();

    const ModelAPI_AttributeValidator* aShapeValidator =
      dynamic_cast<const ModelAPI_AttributeValidator*>(
      aFactory->validator("GeomValidators_ShapeType"));
    std::list<std::string> anArguments;
    anArguments.push_back("circle");
    Events_InfoMessage aCircleError;
    bool aShapeValid = aShapeValidator->isValid(aRefAttr, anArguments, aCircleError);
    // the circle line is not a valid case
    if (aShapeValid) {
      theError = "Circle can not be used in distance constraint";
      return false;
    }

    anArguments.clear();
    anArguments.push_back("line");
    Events_InfoMessage aLineError;
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
                                                Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }

  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  FeaturePtr anAttributeFeature =
    std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

  bool isObject = aRefAttr->isObject();
  ObjectPtr anObject = aRefAttr->object();
  if (isObject && anObject.get()) {
    FeaturePtr aRefFea = ModelAPI_Feature::feature(anObject);

    AttributeRefAttrPtr aOtherAttr = anAttributeFeature->data()->refattr(aParamA);
    ObjectPtr aOtherObject = aOtherAttr->object();
    FeaturePtr aOtherFea = ModelAPI_Feature::feature(aOtherObject);
    if (!aOtherFea)
      return true;

    if (aRefFea->getKind() == SketchPlugin_Line::ID()) {
      if (aOtherFea->getKind() != SketchPlugin_Arc::ID() &&
          aOtherFea->getKind() != SketchPlugin_Circle::ID()) {
        theError = "It refers to a %1, but %2 is neither an %3 nor %4";
        theError.arg(SketchPlugin_Line::ID()).arg(aParamA)
            .arg(SketchPlugin_Arc::ID()).arg(SketchPlugin_Circle::ID());
        return false;
      }
    }
    else if (aRefFea->getKind() == SketchPlugin_Arc::ID() ||
             aRefFea->getKind() == SketchPlugin_Circle::ID()) {
      if (aOtherFea->getKind() != SketchPlugin_Line::ID() &&
          aOtherFea->getKind() != SketchPlugin_Arc::ID() &&
          aOtherFea->getKind() != SketchPlugin_Circle::ID()) {
        theError = "It refers to an %1, but %2 is not a %3 or an %4 or a %5";
        theError.arg(SketchPlugin_Arc::ID()).arg(aParamA)
            .arg(SketchPlugin_Line::ID()).arg(SketchPlugin_Arc::ID())
            .arg(SketchPlugin_Circle::ID());
        return false;
      }
    }
    else {
      theError = "It refers to %1, but should refer to %2 or %3 or %4";
      theError.arg(aRefFea->getKind()).arg(SketchPlugin_Line::ID())
          .arg(SketchPlugin_Arc::ID()).arg(SketchPlugin_Circle::ID());
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
                                             Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
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
        theError = "The object %1 has been already fixed.";
        theError.arg(aName);
        return false;
      }
    }
    else if (aRefAttr->attr() == aRAttr->attr()) {
      AttributePtr anAttribute = aRefAttr->attr();
      std::string aName = anAttribute.get() ? anAttribute->id() : "";
      theError = "The attribute %1 has been already fixed.";
      theError.arg(aName);
      return false;
    }
  }
  return true;
}

bool SketchPlugin_EqualAttrValidator::isValid(const AttributePtr& theAttribute,
                                              const std::list<std::string>& theArguments,
                                              Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
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
      theError = "The %1 feature kind of attribute is wrong. It should be %2 or %3 or %4";
      theError.arg(aFeature->getKind()).arg(SketchPlugin_Line::ID())
          .arg(SketchPlugin_Circle::ID()).arg(SketchPlugin_Arc::ID());
      // wrong type of attribute
      return false;
    }
  }

  if ((aType[0] == SketchPlugin_Line::ID() || aType[1] == SketchPlugin_Line::ID()) &&
      aType[0] != aType[1]) {
    theError = "Feature with kinds %1 and %2 can not be equal.";
    theError.arg(aType[0]).arg(aType[1]);
    return false;
  }
  return true;
}

bool SketchPlugin_MirrorAttrValidator::isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefList::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
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
        theError = "The object %1 is a result of mirror";
        theError.arg(aName);
        return false;
      }
  }
  return true;
}

bool SketchPlugin_CoincidenceAttrValidator::isValid(const AttributePtr& theAttribute,
                                                    const std::list<std::string>& theArguments,
                                                    Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }

  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  FeaturePtr aConstraint = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttrA = aConstraint->data()->refattr(aParamA);
  if (!aRefAttrA) {
    theError = "The %1 attribute should be %2";
    theError.arg(aParamA).arg(ModelAPI_AttributeRefAttr::typeId());
    return false;
  }

  AttributeRefAttrPtr aRefAttrB =
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

  // first attribute is a point, it may coincide with any object
  if (!aRefAttrA->isObject())
    return true;
  else {
    ObjectPtr anObject = aRefAttrA->object();
    if (!anObject.get()) {
      theError = "%1 attribute has an empty object";
      theError.arg(aParamA);
      return false;
    }
    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttrA->object());
    if (!aFeature.get()) {
      theError = "%1 attribute has an empty feature";
      theError.arg(aParamA);
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
      theError = "%1 attribute has an empty object";
      theError.arg(theAttribute->id());
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
                                         Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefList::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
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
        theError = "The object %1 is a result of copy";
        theError.arg(aName);
        return false;
      }
  }
  return true;
}

bool SketchPlugin_SolverErrorValidator::isValid(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  AttributeStringPtr aAttributeString = theFeature->string(SketchPlugin_Sketch::SOLVER_ERROR());

  if (!aAttributeString->value().empty()) {
    theError = aAttributeString->value();
    return false;
  }

  return true;
}

bool SketchPlugin_SolverErrorValidator::isNotObligatory(std::string theFeature,
                                                        std::string theAttribute)
{
  return true;
}

static bool hasSameTangentFeature(const std::set<AttributePtr>& theRefsList,
                                  const FeaturePtr theFeature)
{
  for(std::set<AttributePtr>::const_iterator
      anIt = theRefsList.cbegin(); anIt != theRefsList.cend(); ++anIt) {
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

static bool isPointPointCoincidence(const FeaturePtr& theCoincidence)
{
  AttributeRefAttrPtr aRefAttr[2] = {
      theCoincidence->refattr(SketchPlugin_Constraint::ENTITY_A()),
      theCoincidence->refattr(SketchPlugin_Constraint::ENTITY_B())
  };

  bool arePoints = true;
  for (int i = 0; i < 2 && arePoints; ++i) {
    if (aRefAttr[i]->isObject()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr[i]->object());
      arePoints = aFeature.get() && aFeature->getKind() == SketchPlugin_Point::ID();
    } else
      arePoints = aRefAttr[i]->attr().get() != NULL;
  }
  return arePoints;
}

bool SketchPlugin_FilletVertexValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments,
                                                 Events_InfoMessage& theError) const
{
  AttributeRefAttrPtr aPointRefAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if(!aPointRefAttr.get()) {
    theError = "Error: Point not selected.";
    return false;
  }

  AttributePtr aPointAttribute = aPointRefAttr->attr();
  if (!aPointAttribute.get()) {
    theError = "Error: Bad point selected.";
    return false;
  }
  std::shared_ptr<GeomAPI_Pnt2d> aSelectedPnt =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aPointAttribute)->pnt();

  // Obtain constraint coincidence for the fillet point.
  const std::set<AttributePtr>& aRefsList = aPointAttribute->owner()->data()->refsToMe();
  FeaturePtr aConstraintCoincidence;
  for(std::set<AttributePtr>::const_iterator anIt = aRefsList.cbegin();
      anIt != aRefsList.cend(); ++anIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*anIt);
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
      if (!isPointPointCoincidence(aConstrFeature))
        continue;

      AttributeRefAttrPtr anAttrRefA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_A()));
      AttributeRefAttrPtr anAttrRefB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_B()));

      AttributePtr anAttrA = anAttrRefA->attr();
      if(aPointAttribute == anAttrA) {
        aConstraintCoincidence = aConstrFeature;
        break;
      }

      AttributePtr anAttrB = anAttrRefB->attr();
      if(aPointAttribute == anAttrB) {
        aConstraintCoincidence = aConstrFeature;
        break;
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
                                        aCoinsides,
                                        true);
  SketchPlugin_Tools::findCoincidences(aConstraintCoincidence,
                                        SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                        aCoinsides,
                                        true);

  // Remove points and external lines from set of coincides.
  std::set<FeaturePtr> aNewSetOfCoincides;
  for(std::set<FeaturePtr>::iterator anIt = aCoinsides.begin();
      anIt != aCoinsides.end(); ++anIt) {
    std::shared_ptr<SketchPlugin_SketchEntity> aSketchEntity =
      std::dynamic_pointer_cast<SketchPlugin_SketchEntity>(*anIt);
    if(aSketchEntity.get() && (aSketchEntity->isCopy() || aSketchEntity->isExternal())) {
      continue;
    }
    if((*anIt)->getKind() != SketchPlugin_Line::ID() &&
        (*anIt)->getKind() != SketchPlugin_Arc::ID()) {
          continue;
    }
    if((*anIt)->getKind() == SketchPlugin_Arc::ID()) {
      AttributePtr anArcCenter = (*anIt)->attribute(SketchPlugin_Arc::CENTER_ID());
      std::shared_ptr<GeomAPI_Pnt2d> anArcCenterPnt =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anArcCenter)->pnt();
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
    for(std::set<FeaturePtr>::iterator anIt = aCoinsides.begin();
        anIt != aCoinsides.end(); ++anIt) {
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
  for(std::list<ResultPtr>::iterator aResIt = aFirstResults.begin();
      aResIt != aFirstResults.end(); ++aResIt) {
    ResultPtr aRes = *aResIt;
    const std::set<AttributePtr>& aResRefsList = aRes->data()->refsToMe();
    if(hasSameTangentFeature(aResRefsList, aSecondFeature)) {
      theError = "Error: Edges in selected point has tangent constraint.";
      return false;
    }
  }

  // Check the features are not tangent
  std::shared_ptr<GeomAPI_Shape> aFirstShape = aFirstFeature->lastResult()->shape();
  std::shared_ptr<GeomAPI_Shape> aSecondShape = aSecondFeature->lastResult()->shape();
  if (!aFirstShape || !aFirstShape->isEdge() ||
      !aSecondShape || !aSecondShape->isEdge()) {
    theError = "Error: At least on of the features is not an edge";
    return false;
  }

  std::shared_ptr<GeomAPI_Edge> anEdge1 = std::dynamic_pointer_cast<GeomAPI_Edge>(aFirstShape);
  std::shared_ptr<GeomAPI_Edge> anEdge2 = std::dynamic_pointer_cast<GeomAPI_Edge>(aSecondShape);

  static const double TOL = 1.e-7;
  if (anEdge1->isLine() && anEdge2->isLine()) {
    // Check that lines not collinear
    std::shared_ptr<GeomAPI_Dir> aDir1 = anEdge1->line()->direction();
    std::shared_ptr<GeomAPI_Dir> aDir2 = anEdge2->line()->direction();
    double aCross = aDir1->cross(aDir2)->squareModulus();
    if (aCross < TOL * TOL)
      return false;
  } else if (anEdge1->isArc() && anEdge2->isArc()) {
    // check the circles are not tangent
    std::shared_ptr<GeomAPI_Circ> aCirc1 = anEdge1->circle();
    std::shared_ptr<GeomAPI_Circ> aCirc2 = anEdge2->circle();
    double aDistCC = aCirc1->center()->distance(aCirc2->center());
    double aRadSum = aCirc1->radius() + aCirc2->radius();
    double aRadDiff = fabs(aCirc1->radius() - aCirc2->radius());
    if (fabs(aDistCC - aRadSum) < TOL || fabs(aDistCC - aRadDiff) < TOL)
      return false;
  } else {
    // check whether line and arc are tangent
    std::shared_ptr<GeomAPI_Circ> aCirc;
    std::shared_ptr<GeomAPI_Lin> aLine;
    if (anEdge1->isLine()) {
      aLine = anEdge1->line();
      aCirc = anEdge2->circle();
    } else {
      aCirc = anEdge1->circle();
      aLine = anEdge2->line();
    }

    double aDistCL = aLine->distance(aCirc->center());
    if (fabs(aDistCL - aCirc->radius()) < TOL)
      return false;
  }

  return true;
}

bool SketchPlugin_MiddlePointAttrValidator::isValid(const AttributePtr& theAttribute,
                                                    const std::list<std::string>& theArguments,
                                                    Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }

  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  FeaturePtr anAttributeFeature =
    std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
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
                                                    Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  AttributePtr anAttr = aRefAttr->attr();
  if (!anAttr) {
    theError = "The attribute %1 should be a point";
    theError.arg(theAttribute->id());
    return false;
  }

  FeaturePtr anAttrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->owner());
  const std::string& aFeatureType = anAttrFeature->getKind();
  if (aFeatureType == SketchPlugin_Arc::ID()) {
    // selected point should not be a center of arc
    const std::string& aPntId = anAttr->id();
    if (aPntId != SketchPlugin_Arc::START_ID() && aPntId != SketchPlugin_Arc::END_ID()) {
      theError = "The attribute %1 is not supported";
      theError.arg(aPntId);
      return false;
    }
  }
  else if (aFeatureType == SketchPlugin_Line::ID()) {
    // selected point should be bound point of line
    const std::string& aPntId = anAttr->id();
    if (aPntId != SketchPlugin_Line::START_ID() && aPntId != SketchPlugin_Line::END_ID()) {
      theError = "The attribute %1 is not supported";
      theError.arg(aPntId);
      return false;
    }
  }
  else {
    theError = "Unable to build tangent arc on %1";
    theError.arg(anAttrFeature->getKind());
    return false;
  }

  return true;
}

bool SketchPlugin_IntersectionValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments,
                                                 Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeSelection::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }
  AttributeSelectionPtr aLineAttr =
                       std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  std::shared_ptr<GeomAPI_Edge> anEdge;
  if(aLineAttr && aLineAttr->value() && aLineAttr->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aLineAttr->value()));
  } else if(aLineAttr->context() &&
            aLineAttr->context()->shape() && aLineAttr->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aLineAttr->context()->shape()));
  }

  if (!anEdge || !anEdge->isLine()) {
    theError = "The attribute %1 should be a line";
    theError.arg(theAttribute->id());
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

bool SketchPlugin_SplitValidator::isValid(const AttributePtr& theAttribute,
                                          const std::list<std::string>& theArguments,
                                          Events_InfoMessage& theError) const
{
  bool aValid = false;

  if (theAttribute->attributeType() != ModelAPI_AttributeReference::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return aValid;
  }
  AttributeReferencePtr aFeatureAttr =
            std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);

  ObjectPtr anAttrObject = aFeatureAttr->value();
  FeaturePtr anAttrFeature = ModelAPI_Feature::feature(anAttrObject);
  if (!anAttrFeature)
    return aValid;

  std::string aKind = anAttrFeature->getKind();
  if (aKind == SketchPlugin_Line::ID() ||
      aKind == SketchPlugin_Arc::ID() ||
      aKind == SketchPlugin_Circle::ID()) {

    std::set<ResultPtr> anEdgeShapes;
    ModelGeomAlgo_Shape::shapesOfType(anAttrFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
    if (anEdgeShapes.empty() || anEdgeShapes.size() > 1 /*there case has not existed yet*/)
      return aValid;

    // coincidences to the feature
    std::set<std::shared_ptr<GeomDataAPI_Point2D> > aRefAttributes;
    ModelGeomAlgo_Point2D::getPointsOfReference(anAttrFeature,
                        SketchPlugin_ConstraintCoincidence::ID(),
                        aRefAttributes, SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());

    GeomShapePtr anAttrShape = (*anEdgeShapes.begin())->shape();
    std::shared_ptr<SketchPlugin_Feature> aSFeature =
                                 std::dynamic_pointer_cast<SketchPlugin_Feature>(anAttrFeature);
    SketchPlugin_Sketch* aSketch = aSFeature->sketch();

    std::shared_ptr<ModelAPI_Data> aData = aSketch->data();
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aDirY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));

    typedef std::map<std::shared_ptr<GeomAPI_Pnt>,
                     std::pair<std::list<std::shared_ptr<GeomDataAPI_Point2D> >,
                               std::list<std::shared_ptr<ModelAPI_Object> > > > PointToRefsMap;
    PointToRefsMap aPointsInfo;

    ModelGeomAlgo_Point2D::getPointsInsideShape(anAttrShape, aRefAttributes, aC->pnt(),
                                                aX->dir(), aDirY, aPointsInfo);
    int aCoincidentToFeature = (int)aPointsInfo.size();
    if (aKind == SketchPlugin_Circle::ID())
      aValid = aCoincidentToFeature >= 2;
    else
      aValid = aCoincidentToFeature >= 1;
  }

  return aValid;
}

bool SketchPlugin_TrimValidator::isValid(const AttributePtr& theAttribute,
                                         const std::list<std::string>& theArguments,
                                         Events_InfoMessage& theError) const
{
  bool aValid = false;

  if (theAttribute->attributeType() != ModelAPI_AttributeReference::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return aValid;
  }
  AttributeReferencePtr aBaseObjectAttr =
            std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);

  std::shared_ptr<SketchPlugin_Feature> aTrimFeature =
                 std::dynamic_pointer_cast<SketchPlugin_Feature>(theAttribute->owner());

  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  if (!aBaseObject) {
    AttributePtr aPreviewAttr = aTrimFeature->attribute(SketchPlugin_Trim::PREVIEW_OBJECT());
    aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(aPreviewAttr);
    aBaseObject = aBaseObjectAttr->value();
  }

  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObject);
  if (!aBaseFeature)
    return aValid;

  std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                                 std::dynamic_pointer_cast<SketchPlugin_Feature>(aBaseFeature);
  if (!aSketchFeature.get() || aSketchFeature->isCopy())
    return aValid;

  std::string aKind = aBaseFeature->getKind();
  if (aKind != SketchPlugin_Line::ID() &&
      aKind != SketchPlugin_Arc::ID() &&
      aKind != SketchPlugin_Circle::ID())
    return aValid;

  // point on feature
  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                       aTrimFeature->data()->attribute(SketchPlugin_Trim::PREVIEW_POINT()));

  SketchPlugin_Sketch* aSketch = aTrimFeature->sketch();

  std::shared_ptr<GeomAPI_Pnt2d> anAttributePnt2d = aPoint->pnt();
  std::shared_ptr<GeomAPI_Pnt> anAttributePnt = aSketch->to3D(anAttributePnt2d->x(),
                                                              anAttributePnt2d->y());

  std::map<ObjectPtr, std::set<GeomShapePtr> > aCashedShapes;
  std::map<ObjectPtr, std::map<std::shared_ptr<GeomAPI_Pnt>,
           std::pair<std::list<std::shared_ptr<GeomDataAPI_Point2D> >,
                     std::list<std::shared_ptr<ModelAPI_Object> > > > > anObjectToPoints;
  SketchPlugin_Trim::fillObjectShapes(aBaseObject, aSketch->data()->owner(),
                                      aCashedShapes, anObjectToPoints);
  const std::set<GeomShapePtr>& aShapes = aCashedShapes[aBaseObject];

  return aShapes.size() > 1;
}

bool SketchPlugin_ProjectionValidator::isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeSelection::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }

  AttributeSelectionPtr aFeatureAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<SketchPlugin_Feature> aSketchFeature;
  if (aFeatureAttr.get()) {
    GeomShapePtr aVal = aFeatureAttr->value();
    ResultPtr aRes = aFeatureAttr->context();
    if(aVal && aVal->isEdge()) {
      anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aFeatureAttr->value()));
    } else if(aRes && aRes->shape() && aRes->shape()->isEdge()) {
      anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aFeatureAttr->context()->shape()));
    }

    // try to convert result to sketch feature
    if (aRes) {
      aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(ModelAPI_Feature::feature(aRes));
    }
  }
  if (!anEdge) {
    // check a vertex has been selected
    if (aFeatureAttr->value() && aFeatureAttr->value()->isVertex())
      return true;
    else {
      ResultPtr aRes = aFeatureAttr->context();
      if (aRes && aRes->shape() && aRes->shape()->isVertex())
        return true;
    }

    theError = "The attribute %1 should be an edge or vertex";
    theError.arg(theAttribute->id());
    return false;
  }

  // find a sketch
  std::shared_ptr<SketchPlugin_Sketch> aSketch;
  std::set<AttributePtr> aRefs = theAttribute->owner()->data()->refsToMe();
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
  if (aSketchFeature && aSketch.get() == aSketchFeature->sketch()) {
    theError = "Unable to project feature from the same sketch";
    return false;
  }

  std::shared_ptr<GeomAPI_Pln> aPlane = aSketch->plane();
  std::shared_ptr<GeomAPI_Dir> aNormal = aPlane->direction();
  std::shared_ptr<GeomAPI_Pnt> anOrigin = aPlane->location();

  if (anEdge->isLine()) {
    std::shared_ptr<GeomAPI_Lin> aLine = anEdge->line();
    std::shared_ptr<GeomAPI_Dir> aLineDir = aLine->direction();
    double aDot = fabs(aNormal->dot(aLineDir));
    bool aValid = fabs(aDot - 1.0) >= tolerance * tolerance;
    if (!aValid)
      theError = "Error: Edge is already in the sketch plane.";
    return aValid;
  }
  else if (anEdge->isCircle() || anEdge->isArc()) {
    std::shared_ptr<GeomAPI_Circ> aCircle = anEdge->circle();
    std::shared_ptr<GeomAPI_Dir> aCircNormal = aCircle->normal();
    double aDot = fabs(aNormal->dot(aCircNormal));
    bool aValid = fabs(aDot - 1.0) < tolerance * tolerance;
    if (!aValid)
      theError.arg(anEdge->isCircle() ? "Error: Cirlce is already in the sketch plane."
                                      : "Error: Arc is already in the sketch plane.");
    return aValid;
  }

  theError = "Error: Selected object is not line, circle or arc.";
  return false;
}


static std::set<FeaturePtr> common(const std::set<FeaturePtr>& theSet1,
                                   const std::set<FeaturePtr>& theSet2)
{
  std::set<FeaturePtr> aCommon;
  if (theSet1.empty() || theSet2.empty())
    return aCommon;

  std::set<FeaturePtr>::const_iterator anIt2 = theSet2.begin();
  for (; anIt2 != theSet2.end(); ++anIt2)
    if (theSet1.find(*anIt2) != theSet1.end())
      aCommon.insert(*anIt2);
  return aCommon;
}

bool SketchPlugin_DifferentReferenceValidator::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());

  int aNbFeaturesReferred = 0;
  int aNbAttributesReferred = 0;
  std::set<FeaturePtr> aCommonReferredFeatures;

  // find all features referred by attributes listed in theArguments
  std::list<std::string>::const_iterator anArgIt = theArguments.begin();
  for (; anArgIt != theArguments.end(); ++anArgIt) {
    AttributeRefAttrPtr aRefAttr = anOwner->refattr(*anArgIt);
    if (!aRefAttr)
      continue;

    std::set<FeaturePtr> aCoincidentFeatures;
    if (aRefAttr->isObject()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
      if (aFeature) {
        aCoincidentFeatures.insert(aFeature);
        aNbFeaturesReferred += 1;
      }
    } else {
      AttributePoint2DPtr aPoint =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aRefAttr->attr());
      if (aPoint) {
        aCoincidentFeatures = SketchPlugin_Tools::findFeaturesCoincidentToPoint(aPoint);
        aNbAttributesReferred += 1;
      }
    }

    if (aCommonReferredFeatures.empty())
      aCommonReferredFeatures = aCoincidentFeatures;
    else
      aCommonReferredFeatures = common(aCommonReferredFeatures, aCoincidentFeatures);

    if (aCommonReferredFeatures.empty())
      return true;
  }

  bool isOk = aNbFeaturesReferred < 1;
  if (aNbFeaturesReferred == 1) {
    if (aCommonReferredFeatures.size() == 1) {
      FeaturePtr aFeature = *aCommonReferredFeatures.begin();
      isOk = aNbAttributesReferred <= 1 ||
             aFeature->getKind() == SketchPlugin_Circle::ID() ||
             aFeature->getKind() == SketchPlugin_Arc::ID();
    } else
      isOk = false;
  }

  if (!isOk)
    theError = "Attributes are referred to the same feature";
  return isOk;
}

bool SketchPlugin_DifferentPointReferenceValidator::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  std::set<AttributePoint2DPtr> aReferredCoincidentPoints;

  // find all points referred by attributes listed in theArguments
  bool hasRefsToPoints = false;
  std::list<std::string>::const_iterator anArgIt = theArguments.begin();
  for (; anArgIt != theArguments.end(); ++anArgIt) {
    AttributeRefAttrPtr aRefAttr = anOwner->refattr(*anArgIt);
    if (!aRefAttr)
      continue;

    if (!aRefAttr->isObject()) {
      AttributePoint2DPtr aPoint =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aRefAttr->attr());
      if (aReferredCoincidentPoints.empty())
        aReferredCoincidentPoints = SketchPlugin_Tools::findPointsCoincidentToPoint(aPoint);
      else if (aReferredCoincidentPoints.find(aPoint) == aReferredCoincidentPoints.end())
        return true; // non-coincident point has been found
      else
        hasRefsToPoints = true;
    }
  }

  if (hasRefsToPoints)
    theError = "Attributes are referred to the same point";
  return !hasRefsToPoints;
}

bool SketchPlugin_CirclePassedPointValidator::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>&,
    Events_InfoMessage& theError) const
{
  static const std::string aErrorMessage(
      "Passed point refers to the same feature as a center point");

  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeRefAttrPtr aCenterRef =
      anOwner->refattr(SketchPlugin_MacroCircle::CENTER_POINT_REF_ID());
  AttributeRefAttrPtr aPassedRef =
      anOwner->refattr(SketchPlugin_MacroCircle::PASSED_POINT_REF_ID());

  if (!aPassedRef->isObject())
    return true;

  FeaturePtr aPassedFeature = ModelAPI_Feature::feature(aPassedRef->object());
  if (!aPassedFeature)
    return true;

  if (aCenterRef->isObject()) {
    if (aCenterRef->object() == aPassedRef->object()) {
      theError = aErrorMessage;
      return false;
    }
  } else {
    AttributePoint2DPtr aCenterPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aCenterRef->attr());
    if (aCenterPoint) {
      std::set<FeaturePtr> aCoincidentFeatures =
          SketchPlugin_Tools::findFeaturesCoincidentToPoint(aCenterPoint);
      // check one of coincident features is a feature referred by passed point
      std::set<FeaturePtr>::const_iterator anIt = aCoincidentFeatures.begin();
      for(; anIt != aCoincidentFeatures.end(); ++anIt)
        if (*anIt == aPassedFeature) {
          theError = aErrorMessage;
          return false;
        }
    }
  }
  return true;
}

bool SketchPlugin_ThirdPointValidator::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  return arePointsNotOnLine(anOwner, theError) &&
         arePointsNotSeparated(anOwner, theArguments, theError);
}

static std::shared_ptr<GeomAPI_Pnt2d> toPoint(const FeaturePtr& theMacroCircle,
                                              const std::string& thePointAttrName,
                                              const std::string& theRefPointAttrName)
{
  AttributePoint2DPtr aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theMacroCircle->attribute(thePointAttrName));
  AttributeRefAttrPtr aRefAttr = theMacroCircle->refattr(theRefPointAttrName);

  std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointAttr->pnt();
  if (aRefAttr) {
    if (aRefAttr->isObject()) {
      // project a point onto selected feature
      std::shared_ptr<SketchPlugin_Feature> aFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(
          ModelAPI_Feature::feature(aRefAttr->object()));
      if (aFeature) {
        SketchPlugin_Sketch* aSketch = aFeature->sketch();
        std::shared_ptr<GeomAPI_Edge> anEdge =
            std::dynamic_pointer_cast<GeomAPI_Edge>(aFeature->lastResult()->shape());
        if (anEdge) {
          std::shared_ptr<GeomAPI_Pnt> aPoint3D = aSketch->to3D(aPoint->x(), aPoint->y());
          if (anEdge->isLine())
            aPoint3D = anEdge->line()->project(aPoint3D);
          else if (anEdge->isCircle())
            aPoint3D = anEdge->circle()->project(aPoint3D);
          if(aPoint3D)
            aPoint = aSketch->to2D(aPoint3D);
        }
      }
    } else {
      AttributePoint2DPtr anOtherPoint =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aRefAttr->attr());
      if (anOtherPoint)
        aPoint = anOtherPoint->pnt(); // the reference point is much more precise, use it
    }
  }

  return aPoint;
}

static void threePointsOfFeature(const FeaturePtr& theMacroFeature,
                                 std::shared_ptr<GeomAPI_Pnt2d> thePoints[3])
{
  if (theMacroFeature->getKind() == SketchPlugin_MacroCircle::ID()) {
    thePoints[0] = toPoint(theMacroFeature,
          SketchPlugin_MacroCircle::FIRST_POINT_ID(),
          SketchPlugin_MacroCircle::FIRST_POINT_REF_ID());
    thePoints[1] = toPoint(theMacroFeature,
          SketchPlugin_MacroCircle::SECOND_POINT_ID(),
          SketchPlugin_MacroCircle::SECOND_POINT_REF_ID());
    thePoints[2] = toPoint(theMacroFeature,
          SketchPlugin_MacroCircle::THIRD_POINT_ID(),
          SketchPlugin_MacroCircle::THIRD_POINT_REF_ID());
  } else if (theMacroFeature->getKind() == SketchPlugin_MacroArc::ID()) {
    thePoints[0] = toPoint(theMacroFeature,
          SketchPlugin_MacroArc::START_POINT_2_ID(),
          SketchPlugin_MacroArc::START_POINT_REF_ID());
    thePoints[1] = toPoint(theMacroFeature,
          SketchPlugin_MacroArc::END_POINT_2_ID(),
          SketchPlugin_MacroArc::END_POINT_REF_ID());
    thePoints[2] = toPoint(theMacroFeature,
          SketchPlugin_MacroArc::PASSED_POINT_ID(),
          SketchPlugin_MacroArc::PASSED_POINT_REF_ID());
  }
}

static bool isPointsOnLine(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                           const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                           const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
{
  static const double aTolerance = 1.e-7;
  if (thePoint1->distance(thePoint2) < aTolerance ||
      thePoint1->distance(thePoint3) < aTolerance)
    return true;

  std::shared_ptr<GeomAPI_Dir2d> aDirP1P2(new GeomAPI_Dir2d(thePoint2->x() - thePoint1->x(),
                                                            thePoint2->y() - thePoint1->y()));
  std::shared_ptr<GeomAPI_Dir2d> aDirP1P3(new GeomAPI_Dir2d(thePoint3->x() - thePoint1->x(),
                                                            thePoint3->y() - thePoint1->y()));
  return fabs(aDirP1P2->cross(aDirP1P3)) < aTolerance;
}

static bool isOnSameSide(const std::shared_ptr<GeomAPI_Lin>& theLine,
                         const std::shared_ptr<GeomAPI_Pnt>& thePoint1,
                         const std::shared_ptr<GeomAPI_Pnt>& thePoint2)
{
  static const double aTolerance = 1.e-7;
  std::shared_ptr<GeomAPI_Dir> aLineDir = theLine->direction();
  std::shared_ptr<GeomAPI_XYZ> aLineLoc = theLine->location()->xyz();

  std::shared_ptr<GeomAPI_XYZ> aVec1 = thePoint1->xyz()->decreased(aLineLoc);
  // the first point is on the line
  if (aVec1->squareModulus() < aTolerance * aTolerance)
    return false;
  std::shared_ptr<GeomAPI_Dir> aDirP1L(new GeomAPI_Dir(aVec1));
  std::shared_ptr<GeomAPI_XYZ> aVec2 = thePoint2->xyz()->decreased(aLineLoc);
  // the second point is on the line
  if (aVec2->squareModulus() < aTolerance * aTolerance)
    return false;
  std::shared_ptr<GeomAPI_Dir> aDirP2L(new GeomAPI_Dir(aVec2));

  return aLineDir->cross(aDirP1L)->dot(aLineDir->cross(aDirP2L)) > -aTolerance;
}

static bool isOnSameSide(const std::shared_ptr<GeomAPI_Circ>& theCircle,
                         const std::shared_ptr<GeomAPI_Pnt>&  thePoint1,
                         const std::shared_ptr<GeomAPI_Pnt>&  thePoint2)
{
  static const double aTolerance = 1.e-7;
  std::shared_ptr<GeomAPI_Pnt> aCenter = theCircle->center();
  double aDistP1C = thePoint1->distance(aCenter);
  double aDistP2C = thePoint2->distance(aCenter);
  return (aDistP1C - theCircle->radius()) * (aDistP2C - theCircle->radius()) > -aTolerance;
}

bool SketchPlugin_ThirdPointValidator::arePointsNotOnLine(
    const FeaturePtr& theMacroFeature,
    Events_InfoMessage& theError) const
{
  static const std::string aErrorPointsOnLine(
      "Selected points are on the same line");

  std::shared_ptr<GeomAPI_Pnt2d> aPoints[3];
  threePointsOfFeature(theMacroFeature, aPoints);

  if (isPointsOnLine(aPoints[0], aPoints[1], aPoints[2])) {
    theError = aErrorPointsOnLine;
    return false;
  }
  return true;
}

bool SketchPlugin_ThirdPointValidator::arePointsNotSeparated(
    const FeaturePtr& theMacroFeature,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  static const std::string aErrorPointsApart(
      "Selected entity is lying between first two points");

  AttributeRefAttrPtr aThirdPointRef = theMacroFeature->refattr(theArguments.front());
  FeaturePtr aRefByThird;
  if (aThirdPointRef->isObject())
    aRefByThird = ModelAPI_Feature::feature(aThirdPointRef->object());
  if (!aRefByThird)
    return true;

  std::shared_ptr<GeomAPI_Pnt2d> aPoints[3];
  threePointsOfFeature(theMacroFeature, aPoints);

  std::shared_ptr<GeomAPI_Edge> aThirdShape =
      std::dynamic_pointer_cast<GeomAPI_Edge>(aRefByThird->lastResult()->shape());
  if (!aThirdShape)
    return true;

  SketchPlugin_Sketch* aSketch =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theMacroFeature)->sketch();
  std::shared_ptr<GeomAPI_Pnt> aFirstPnt3D = aSketch->to3D(aPoints[0]->x(), aPoints[0]->y());
  std::shared_ptr<GeomAPI_Pnt> aSecondPnt3D = aSketch->to3D(aPoints[1]->x(), aPoints[1]->y());

  bool isOk = true;
  if (aThirdShape->isLine())
    isOk = isOnSameSide(aThirdShape->line(), aFirstPnt3D, aSecondPnt3D);
  else if (aThirdShape->isCircle() || aThirdShape->isArc())
    isOk = isOnSameSide(aThirdShape->circle(), aFirstPnt3D, aSecondPnt3D);

  if (!isOk)
    theError = aErrorPointsApart;
  return isOk;
}

bool SketchPlugin_ArcEndPointValidator::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
  AttributeRefAttrPtr anEndPointRef = aFeature->refattr(theArguments.front());

  if(!anEndPointRef.get()) {
    return true;
  }

  ObjectPtr anObject = anEndPointRef->object();
  AttributePtr anAttr = anEndPointRef->attr();
  if(!anObject.get() && !anAttr.get()) {
    return true;
  }

  if(anEndPointRef->attr().get()) {
    return false;
  }

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  if(aResult.get()) {
    GeomShapePtr aShape = aResult->shape();
    if(aShape.get() && aShape->isVertex()) {
      return false;
    }
  }

  aFeature = ModelAPI_Feature::feature(anObject);
  if(aFeature.get()) {
    if(aFeature->getKind() == SketchPlugin_Point::ID()) {
      return false;
    }
  }

  return true;
}

static GeomShapePtr toInfiniteEdge(const GeomShapePtr theShape)
{
  if(!theShape.get()) {
    return theShape;
  }

  if(!theShape->isEdge()) {
    return theShape;
  }

  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(theShape));

  if(!anEdge.get()) {
    return theShape;
  }

  if(anEdge->isLine()) {
    std::shared_ptr<GeomAPI_Lin> aLine = anEdge->line();
    GeomShapePtr aShape = GeomAlgoAPI_EdgeBuilder::line(aLine);
    return aShape;
  }

  if(anEdge->isCircle() || anEdge->isArc()) {
    std::shared_ptr<GeomAPI_Circ> aCircle = anEdge->circle();
    GeomShapePtr aShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCircle);
    return aShape;
  }

  return theShape;
}

bool SketchPlugin_ArcEndPointIntersectionValidator::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  std::shared_ptr<SketchPlugin_MacroArc> anArcFeature =
      std::dynamic_pointer_cast<SketchPlugin_MacroArc>(theAttribute->owner());
  AttributeRefAttrPtr anEndPointRef = anArcFeature->refattr(theArguments.front());

  if(!anEndPointRef.get()) {
    return true;
  }

  GeomShapePtr anArcShape = toInfiniteEdge(anArcFeature->getArcShape(false));

  if(!anArcShape.get() || anArcShape->isNull()) {
    return true;
  }

  ObjectPtr anObject = anEndPointRef->object();
  AttributePtr anAttr = anEndPointRef->attr();
  if(!anObject.get() && !anAttr.get()) {
    return true;
  }

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  if(aResult.get()) {
    GeomShapePtr aShape = aResult->shape();
    if (!aShape->isEdge())
      return true;
    aShape = toInfiniteEdge(aShape);
    if(aShape.get() && !aShape->isNull()) {
      if(anArcShape->isIntersect(aShape)) {
        return true;
      }
    }
  }

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(anObject);
  if(aSelectedFeature.get()) {
    std::list<ResultPtr> aResults = aSelectedFeature->results();
    for(std::list<ResultPtr>::const_iterator anIt = aResults.cbegin();
        anIt != aResults.cend();
        ++anIt)
    {
      GeomShapePtr aShape = (*anIt)->shape();
      if (!aShape->isEdge())
        return true;
      aShape = toInfiniteEdge(aShape);
      if(aShape.get() && !aShape->isNull()) {
        if(anArcShape->isIntersect(aShape)) {
          return true;
        }
      }
    }
  }

  return false;
}

bool SketchPlugin_HasNoConstraint::isValid(const AttributePtr& theAttribute,
                                           const std::list<std::string>& theArguments,
                                           Events_InfoMessage& theError) const
{
  std::set<std::string> aFeatureKinds;
  for (std::list<std::string>::const_iterator anArgIt = theArguments.begin();
       anArgIt != theArguments.end(); anArgIt++) {
    aFeatureKinds.insert(*anArgIt);
  }

  if (theAttribute->attributeType() != ModelAPI_AttributeRefAttr::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }

  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
                                                                      (theAttribute);
  bool isObject = aRefAttr->isObject();
  if (!isObject) {
    theError = "It uses an empty object";
    return false;
  }
  ObjectPtr anObject = aRefAttr->object();
  FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
  if (!aFeature.get()) {
    theError = "The feature of the checked attribute is empty";
    return false;
  }

  FeaturePtr aCurrentFeature = ModelAPI_Feature::feature(aRefAttr->owner());

  std::set<AttributePtr> aRefsList = anObject->data()->refsToMe();
  std::set<AttributePtr>::const_iterator anIt = aRefsList.begin();
  for (; anIt != aRefsList.end(); anIt++) {
    FeaturePtr aRefFeature = ModelAPI_Feature::feature((*anIt)->owner());
    if (aRefFeature.get() && aCurrentFeature != aRefFeature &&
        aFeatureKinds.find(aRefFeature->getKind()) != aFeatureKinds.end())
      return false; // constraint is found, that means that the check is not valid
  }
  return true;
}

bool SketchPlugin_ReplicationReferenceValidator::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (!aRefAttr)
  {
    theError = "Incorrect attribute";
    return false;
  }

  ObjectPtr anOwner;
  if (aRefAttr->isObject())
    anOwner = aRefAttr->object();
  else
  {
    AttributePtr anAttr = aRefAttr->attr();
    anOwner = anAttr->owner();
  }
  FeaturePtr anAttrOwnerFeature = ModelAPI_Feature::feature(anOwner);
  if (!anAttrOwnerFeature)
    return true;
  AttributeBooleanPtr aCopyAttr = anAttrOwnerFeature->boolean(SketchPlugin_SketchEntity::COPY_ID());
  if (!aCopyAttr || !aCopyAttr->value())
    return true; // feature is not a copy, thus valid

  // check the copy feature is already referred by the "Multi" feature
  FeaturePtr aMultiFeature = ModelAPI_Feature::feature(theAttribute->owner());
  AttributeRefListPtr aRefList = aMultiFeature->reflist(theArguments.front());
  for (int i = 0; i < aRefList->size(); ++i)
  {
    FeaturePtr aRefOwner = ModelAPI_Feature::feature(aRefList->object(i));
    if (aRefOwner == anAttrOwnerFeature)
    {
      theError = "Attribute refers to the object generated by this feature";
      return false;
    }
  }

  return true;
}
