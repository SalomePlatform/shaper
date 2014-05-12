// File:    SketchPlugin_ConstraintDistance.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintDistance.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>

SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
  myAttrList.push_back(CONSTRAINT_ATTR_DISTANCE);
}

void SketchPlugin_ConstraintDistance::execute()
{
}


void SketchPlugin_ConstraintDistance::setAttributes(
          const double                                  theDistance, 
          const boost::shared_ptr<SketchPlugin_Feature> theEntityA, 
          const boost::shared_ptr<SketchPlugin_Feature> theEntityB)
{
  // Assign the value of the distance
  data()->addAttribute(CONSTRAINT_ATTR_DISTANCE, ModelAPI_AttributeDouble::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
    data()->attribute(CONSTRAINT_ATTR_DISTANCE))->setValue(theDistance);

  // Assign parameters of the constraint
  std::string aPoints[2] = {CONSTRAINT_ATTR_POINT_A, CONSTRAINT_ATTR_POINT_B};
  std::string anEntities[2] = {CONSTRAINT_ATTR_ENTITY_A, CONSTRAINT_ATTR_ENTITY_B};
  int aCurPt = 0;
  int aCurEnt = 0;
  std::string aCurAttr;
  // add entityA depending on its type
  if (theEntityA->getKind() == SketchPlugin_Point().getKind())
    aCurAttr = aPoints[aCurPt++];
  else
    aCurAttr = anEntities[aCurEnt++];
  myAttrList.push_back(aCurAttr);
  data()->addAttribute(aCurAttr, ModelAPI_AttributeReference::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(
    data()->attribute(aCurAttr))->setValue(theEntityA);
  // add entityB depending on its type
  if (theEntityB->getKind() == SketchPlugin_Point().getKind())
    aCurAttr = aPoints[aCurPt++];
  else
    aCurAttr = anEntities[aCurEnt++];
  myAttrList.push_back(aCurAttr);
  data()->addAttribute(aCurAttr, ModelAPI_AttributeReference::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(
    data()->attribute(aCurAttr))->setValue(theEntityB);
}

void SketchPlugin_ConstraintDistance::setAttributes(
          const double                                  theDistance, 
          const boost::shared_ptr<ModelAPI_Attribute>   thePoint, 
          const boost::shared_ptr<SketchPlugin_Feature> theEntity)
{
  // Assign the value of the distance
  data()->addAttribute(CONSTRAINT_ATTR_DISTANCE, ModelAPI_AttributeDouble::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
    data()->attribute(CONSTRAINT_ATTR_DISTANCE))->setValue(theDistance);

  // Assign reference to the first point
  myAttrList.push_back(CONSTRAINT_ATTR_POINT_A);
  data()->addAttribute(CONSTRAINT_ATTR_POINT_A, ModelAPI_AttributeRefAttr::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
    data()->attribute(CONSTRAINT_ATTR_POINT_A))->setValue(thePoint);

  // Assign reference to the entity
  std::string aCurAttr;
  if (theEntity->getKind() == SketchPlugin_Point().getKind())
    aCurAttr = CONSTRAINT_ATTR_POINT_B;
  else
    aCurAttr = CONSTRAINT_ATTR_ENTITY_A;
  myAttrList.push_back(aCurAttr);
  data()->addAttribute(aCurAttr, ModelAPI_AttributeReference::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(
    data()->attribute(aCurAttr))->setValue(theEntity);
}

void SketchPlugin_ConstraintDistance::setAttributes(
          const double                                  theDistance, 
          const boost::shared_ptr<ModelAPI_Attribute>   thePointA, 
          const boost::shared_ptr<ModelAPI_Attribute>   thePointB)
{
  // Assign the value of the distance
  data()->addAttribute(CONSTRAINT_ATTR_DISTANCE, ModelAPI_AttributeDouble::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
    data()->attribute(CONSTRAINT_ATTR_DISTANCE))->setValue(theDistance);

  // Assign reference to the first point
  myAttrList.push_back(CONSTRAINT_ATTR_POINT_A);
  data()->addAttribute(CONSTRAINT_ATTR_POINT_A, ModelAPI_AttributeRefAttr::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
    data()->attribute(CONSTRAINT_ATTR_POINT_A))->setValue(thePointA);

  // Assign reference to the second point
  myAttrList.push_back(CONSTRAINT_ATTR_POINT_B);
  data()->addAttribute(CONSTRAINT_ATTR_POINT_B, ModelAPI_AttributeRefAttr::type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
    data()->attribute(CONSTRAINT_ATTR_POINT_B))->setValue(thePointB);
}

void SketchPlugin_ConstraintDistance::setAttributes(
          const double                                  theRadius, 
          const boost::shared_ptr<SketchPlugin_Feature> theCircle)
{
   /// \todo Need to be implemented
}

void SketchPlugin_ConstraintDistance::setAttributes(
          const double                                  theDistance, 
          const boost::shared_ptr<SketchPlugin_Feature> thePointA, 
          const boost::shared_ptr<SketchPlugin_Feature> thePointB, 
          const boost::shared_ptr<SketchPlugin_Feature> theEntity)
{
   /// \todo Need to be implemented. Possibly need to add points by their attributes
}


