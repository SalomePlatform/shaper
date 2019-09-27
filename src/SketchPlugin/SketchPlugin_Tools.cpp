// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "SketchPlugin_Tools.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintCoincidenceInternal.h"
#include "SketchPlugin_ConstraintLength.h"
#include "SketchPlugin_ConstraintTangent.h"
#include "SketchPlugin_Ellipse.h"
#include "SketchPlugin_EllipticArc.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Projection.h"
#include "SketchPlugin_SketchEntity.h"
#include "SketchPlugin_Split.h"
#include "SketchPlugin_Trim.h"

#include <SketcherPrs_Tools.h>

#include <ModelAPI_AttributeDouble.h>

#include <ModelGeomAlgo_Point2D.h>
#include <ModelGeomAlgo_Shape.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#ifdef DEBUG_TRIM
#include <iostream>
#endif

namespace SketchPlugin_Tools {

void clearExpressions(AttributeDoublePtr theAttribute)
{
  theAttribute->setText(std::string());
}

void clearExpressions(AttributePointPtr theAttribute)
{
  theAttribute->setText(std::string(), std::string(), std::string());
}

void clearExpressions(AttributePoint2DPtr theAttribute)
{
  theAttribute->setText(std::string(), std::string());
}

void clearExpressions(AttributePtr theAttribute)
{
  // Double
  AttributeDoublePtr anAttributeDouble =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (anAttributeDouble.get())
    clearExpressions(anAttributeDouble);
  // Point
  AttributePointPtr anAttributePoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);
  if (anAttributePoint.get())
    clearExpressions(anAttributePoint);
  // Point2D
  AttributePoint2DPtr anAttributePoint2D =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (anAttributePoint2D.get())
    clearExpressions(anAttributePoint2D);
}

void clearExpressions(FeaturePtr theFeature)
{
  if (!theFeature.get())
    return;

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::iterator anAttributeIt = anAttributes.begin();
  for (; anAttributeIt != anAttributes.end(); ++anAttributeIt) {
    clearExpressions(*anAttributeIt);
  }
}

std::shared_ptr<GeomAPI_Pnt2d> getCoincidencePoint(const FeaturePtr theStartCoin)
{
  std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(theStartCoin.get(),
                                                          SketchPlugin_Constraint::ENTITY_A());
  if (aPnt.get() == NULL)
    aPnt = SketcherPrs_Tools::getPoint(theStartCoin.get(), SketchPlugin_Constraint::ENTITY_B());
  return aPnt;
}

std::set<FeaturePtr> findCoincidentConstraints(const FeaturePtr& theFeature)
{
  std::set<FeaturePtr> aCoincident;
  const std::set<AttributePtr>& aRefsList = theFeature->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aIt)->owner());
    if (aConstrFeature && aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID())
      aCoincident.insert(aConstrFeature);
  }
  return aCoincident;
}

void findCoincidences(const FeaturePtr theStartCoin,
                      const std::string& theAttr,
                      std::set<FeaturePtr>& theList,
                      const bool theIsAttrOnly)
{
  AttributeRefAttrPtr aPnt = theStartCoin->refattr(theAttr);
  if(!aPnt) {
    return;
  }
  FeaturePtr aObj = ModelAPI_Feature::feature(aPnt->object());
  if(theList.find(aObj) == theList.end()) {
    std::shared_ptr<GeomAPI_Pnt2d> aOrig = getCoincidencePoint(theStartCoin);
    if(aOrig.get() == NULL) {
      return;
    }
    if(!theIsAttrOnly || !aPnt->isObject()) {
      theList.insert(aObj);
    }
    std::set<FeaturePtr> aCoincidences = findCoincidentConstraints(aObj);
    std::set<FeaturePtr>::const_iterator aCIt = aCoincidences.begin();
    for (; aCIt != aCoincidences.end(); ++aCIt) {
      FeaturePtr aConstrFeature = *aCIt;
      std::shared_ptr<GeomAPI_Pnt2d> aPnt = getCoincidencePoint(aConstrFeature);
      if(aPnt.get() && aOrig->isEqual(aPnt)) {
        findCoincidences(aConstrFeature, SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                         theList, theIsAttrOnly);
        findCoincidences(aConstrFeature, SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                         theList, theIsAttrOnly);
      }
    }
  }
}

std::set<FeaturePtr> findFeaturesCoincidentToPoint(const AttributePoint2DPtr& thePoint)
{
  std::set<FeaturePtr> aCoincidentFeatures;

  FeaturePtr anOwner = ModelAPI_Feature::feature(thePoint->owner());
  aCoincidentFeatures.insert(anOwner);

  std::set<FeaturePtr> aCoincidences = findCoincidentConstraints(anOwner);
  std::set<FeaturePtr>::const_iterator aCIt = aCoincidences.begin();
  for (; aCIt != aCoincidences.end(); ++aCIt) {
    bool isPointUsedInCoincidence = false;
    AttributeRefAttrPtr anOtherCoincidentAttr;
    for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
      AttributeRefAttrPtr aRefAttr = (*aCIt)->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
      if (!aRefAttr)
        continue;
      if (!aRefAttr->isObject() && aRefAttr->attr() == thePoint)
        isPointUsedInCoincidence = true;
      else
        anOtherCoincidentAttr = aRefAttr;
    }

    if (isPointUsedInCoincidence) {
      ObjectPtr anObj;
      if (anOtherCoincidentAttr->isObject())
        anObj = anOtherCoincidentAttr->object();
      else
        anObj = anOtherCoincidentAttr->attr()->owner();
      aCoincidentFeatures.insert(ModelAPI_Feature::feature(anObj));
    }
  }

  return aCoincidentFeatures;
}

// Container for point-point coincidences.
// Useful to find points coincident to a given point.
class CoincidentPoints
{
public:
  void addCoincidence(const AttributePoint2DPtr& thePoint1,
                      const AttributePoint2DPtr& thePoint2 = AttributePoint2DPtr())
  {
    std::list< std::set<AttributePoint2DPtr> >::iterator aFound1 = find(thePoint1);
    std::list< std::set<AttributePoint2DPtr> >::iterator aFound2 = find(thePoint2);
    if (aFound1 == myCoincidentPoints.end()) {
      if (aFound2 == myCoincidentPoints.end()) {
        std::set<AttributePoint2DPtr> aNewSet;
        aNewSet.insert(thePoint1);
        if (thePoint2)
          aNewSet.insert(thePoint2);
        myCoincidentPoints.push_back(aNewSet);
      } else
        aFound2->insert(thePoint1);
    } else if (aFound2 == myCoincidentPoints.end()) {
      if (thePoint2)
        aFound1->insert(thePoint2);
    } else {
      aFound1->insert(aFound2->begin(), aFound2->end());
      myCoincidentPoints.erase(aFound2);
    }
  }

  std::set<AttributePoint2DPtr> coincidentPoints(const AttributePoint2DPtr& thePoint)
  {
    collectCoincidentPoints(thePoint);

    std::list< std::set<AttributePoint2DPtr> >::iterator aFound = find(thePoint);
    if (aFound == myCoincidentPoints.end())
      return std::set<AttributePoint2DPtr>();
    return *aFound;
  }

private:
  void coincidences(const FeaturePtr& theFeature,
                    std::set<FeaturePtr>& theCoincidences) const
  {
    // iterate through coincideces for the given feature
    std::set<FeaturePtr> aCoincidences = SketchPlugin_Tools::findCoincidentConstraints(theFeature);
    std::set<FeaturePtr>::const_iterator aCIt = aCoincidences.begin();
    for (; aCIt != aCoincidences.end(); ++aCIt)
    {
      if (theCoincidences.find(*aCIt) != theCoincidences.end())
        continue; // already processed
      theCoincidences.insert(*aCIt);
      // iterate on coincident attributes
      for (int i = 0, aPtInd = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
        AttributeRefAttrPtr aRefAttr = (*aCIt)->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
        if (aRefAttr && !aRefAttr->isObject())
        {
          FeaturePtr anOwner = ModelAPI_Feature::feature(aRefAttr->attr()->owner());
          if (anOwner != theFeature)
            coincidences(anOwner, theCoincidences);
        }
      }
    }
  }

  // Iteratively search points coincident to the given point
  // (two points may be coincident through the third point)
  void collectCoincidentPoints(const AttributePoint2DPtr& thePoint)
  {
    AttributePoint2DPtr aPoints[2];

    FeaturePtr anOwner = ModelAPI_Feature::feature(thePoint->owner());
    std::set<FeaturePtr> aCoincidences;
    coincidences(anOwner, aCoincidences);

    std::set<FeaturePtr>::const_iterator aCIt = aCoincidences.begin();
    for (; aCIt != aCoincidences.end(); ++aCIt) {
      aPoints[0] = AttributePoint2DPtr();
      aPoints[1] = AttributePoint2DPtr();
      for (int i = 0, aPtInd = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
        AttributeRefAttrPtr aRefAttr = (*aCIt)->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
        if (aRefAttr && !aRefAttr->isObject())
          aPoints[aPtInd++] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aRefAttr->attr());
      }

      if (aPoints[0] && aPoints[1])
        addCoincidence(aPoints[0], aPoints[1]);
    }
  }

  std::list< std::set<AttributePoint2DPtr> >::iterator find(const AttributePoint2DPtr& thePoint)
  {
    std::list< std::set<AttributePoint2DPtr> >::iterator aSeek = myCoincidentPoints.begin();
    for (; aSeek != myCoincidentPoints.end(); ++aSeek)
      if (aSeek->find(thePoint) != aSeek->end())
        return aSeek;
    return myCoincidentPoints.end();
  }

private:
  std::list< std::set<AttributePoint2DPtr> > myCoincidentPoints;
};

std::set<AttributePoint2DPtr> findPointsCoincidentToPoint(const AttributePoint2DPtr& thePoint)
{
  CoincidentPoints aCoincidentPoints;
  return aCoincidentPoints.coincidentPoints(thePoint);
}

void resetAttribute(SketchPlugin_Feature* theFeature,
                    const std::string& theId)
{
  AttributePtr anAttr = theFeature->attribute(theId);
  if(anAttr.get()) {
    anAttr->reset();
  }
}

void createCoincidenceOrTangency(SketchPlugin_Feature* theFeature,
                                 const std::string& theId,
                                 const AttributePtr theAttr,
                                 const ObjectPtr theObject,
                                 const bool theIsCanBeTangent)
{
  AttributeRefAttrPtr aRefAttr = theFeature->refattr(theId);
  if(aRefAttr.get() && aRefAttr->isInitialized()) {
    FeaturePtr aConstraint;
    if(!theIsCanBeTangent) {
      aConstraint = theFeature->sketch()
                              ->addFeature(SketchPlugin_ConstraintCoincidence::ID());
    } else {
      if(aRefAttr->isObject()) {
        ObjectPtr anObject = aRefAttr->object();
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
        if(aFeature->getKind() == SketchPlugin_Point::ID()) {
          aConstraint = theFeature->sketch()
                                  ->addFeature(SketchPlugin_ConstraintCoincidence::ID());
        } else {
          aConstraint = theFeature->sketch()
                                  ->addFeature(SketchPlugin_ConstraintTangent::ID());
        }
      } else {
        aConstraint = theFeature->sketch()
                                ->addFeature(SketchPlugin_ConstraintCoincidence::ID());
      }
    }
    AttributeRefAttrPtr aRefAttrA = aConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
    aRefAttr->isObject() ? aRefAttrA->setObject(aRefAttr->object())
                         : aRefAttrA->setAttr(aRefAttr->attr());
    AttributeRefAttrPtr aRefAttrB = aConstraint->refattr(SketchPlugin_Constraint::ENTITY_B());
    if(theObject.get()) {
      aRefAttrB->setObject(theObject);
    } else if(theAttr.get()) {
      aRefAttrB->setAttr(theAttr);
    }
  }
}

void convertRefAttrToPointOrTangentCurve(const AttributeRefAttrPtr&      theRefAttr,
                                         const AttributePtr&             theDefaultAttr,
                                         std::shared_ptr<GeomAPI_Shape>& theTangentCurve,
                                         std::shared_ptr<GeomAPI_Pnt2d>& thePassingPoint)
{
  AttributePtr anAttr = theDefaultAttr;
  if (theRefAttr->isObject()) {
    FeaturePtr aTgFeature = ModelAPI_Feature::feature(theRefAttr->object());
    if (aTgFeature) {
      if (aTgFeature->getKind() != SketchPlugin_Point::ID()) {
        theTangentCurve = aTgFeature->lastResult()->shape();
        return;
      }
      anAttr = aTgFeature->attribute(SketchPlugin_Point::COORD_ID());
    }
  } else
    anAttr = theRefAttr->attr();

  thePassingPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr)->pnt();
}


FeaturePtr createConstraintAttrAttr(SketchPlugin_Sketch* theSketch,
                                    const std::string& theConstraintId,
                                    const AttributePtr& theFirstAttribute,
                                    const AttributePtr& theSecondAttribute)
{
  FeaturePtr aConstraint = theSketch->addFeature(theConstraintId);
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setAttr(theFirstAttribute);

  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setAttr(theSecondAttribute);

#if defined(DEBUG_TRIM) || defined(DEBUG_SPLIT)
  std::cout << "<createConstraint to attribute> :"
            << " first attribute - " << theFirstAttribute->id()
            << " second attribute - " << theSecondAttribute->id()
            << std::endl;
#endif

  return aConstraint;
}

FeaturePtr createConstraintAttrObject(SketchPlugin_Sketch* theSketch,
                                      const std::string& theConstraintId,
                                      const AttributePtr& theFirstAttribute,
                                      const ObjectPtr& theSecondObject)
{
  FeaturePtr aConstraint = theSketch->addFeature(theConstraintId);
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setAttr(theFirstAttribute);

  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setObject(theSecondObject);

#if defined(DEBUG_TRIM) || defined(DEBUG_SPLIT)
  std::cout << "<createConstraint to attribute> :"
            << " first attribute - " << theFirstAttribute->id()
            << " second object - " << ModelAPI_Feature::feature(theSecondObject)->getKind()
            << std::endl;
#endif

  return aConstraint;
}

FeaturePtr createConstraintObjectObject(SketchPlugin_Sketch* theSketch,
                                        const std::string& theConstraintId,
                                        const ObjectPtr& theFirstObject,
                                        const ObjectPtr& theSecondObject)
{
  FeaturePtr aConstraint = theSketch->addFeature(theConstraintId);
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setObject(theFirstObject);

  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setObject(theSecondObject);

#if defined(DEBUG_TRIM) || defined(DEBUG_SPLIT)
  std::cout << "<createConstraint to attribute> :"
            << " first object - " << ModelAPI_Feature::feature(theFirstObject)->getKind()
            << " second object - " << ModelAPI_Feature::feature(theSecondObject)->getKind()
            << std::endl;
#endif

  return aConstraint;
}

void createAuxiliaryPointOnEllipse(const FeaturePtr& theEllipseFeature,
                                   const std::string& theEllipsePoint)
{
  SketchPlugin_Sketch* aSketch =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theEllipseFeature)->sketch();

  FeaturePtr aPointFeature = aSketch->addFeature(SketchPlugin_Point::ID());
  aPointFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
  aPointFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(theEllipseFeature);

  AttributePoint2DPtr anElPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    theEllipseFeature->attribute(theEllipsePoint));

  AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aPointFeature->attribute(SketchPlugin_Point::COORD_ID()));
  aCoord->setValue(anElPoint->x(), anElPoint->y());

  aPointFeature->execute();
  std::string aName = theEllipseFeature->name() + "_" + theEllipsePoint;
  aPointFeature->data()->setName(aName);
  aPointFeature->lastResult()->data()->setName(aName);

  createConstraintAttrAttr(aSketch,
      SketchPlugin_ConstraintCoincidenceInternal::ID(), anElPoint, aCoord);
}

void createAuxiliaryAxisOfEllipse(const FeaturePtr& theEllipseFeature,
                                  const std::string& theStartPoint,
                                  const std::string& theEndPoint)
{
  SketchPlugin_Sketch* aSketch =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theEllipseFeature)->sketch();

  FeaturePtr aLineFeature = aSketch->addFeature(SketchPlugin_Line::ID());
  aLineFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
  aLineFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(theEllipseFeature);

  AttributePoint2DPtr aStartPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    theEllipseFeature->attribute(theStartPoint));
  AttributePoint2DPtr aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    theEllipseFeature->attribute(theEndPoint));

  AttributePoint2DPtr aLineStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aLineFeature->attribute(SketchPlugin_Line::START_ID()));
  aLineStart->setValue(aStartPoint->x(), aStartPoint->y());

  AttributePoint2DPtr aLineEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aLineFeature->attribute(SketchPlugin_Line::END_ID()));
  aLineEnd->setValue(aEndPoint->x(), aEndPoint->y());

  aLineFeature->execute();
  std::string aName = theEllipseFeature->name() + "_" +
    (theStartPoint == SketchPlugin_Ellipse::MAJOR_AXIS_START_ID() ? "major_axis" : "minor_axis");
  aLineFeature->data()->setName(aName);
  aLineFeature->lastResult()->data()->setName(aName);

  createConstraintAttrAttr(aSketch,
      SketchPlugin_ConstraintCoincidenceInternal::ID(), aStartPoint, aLineStart);
  createConstraintAttrAttr(aSketch,
      SketchPlugin_ConstraintCoincidenceInternal::ID(), aEndPoint, aLineEnd);
}

GeomPnt2dPtr flyoutPointCoordinates(const ConstraintPtr& theConstraint)
{
  // currently process Length constraints only
  if (theConstraint->getKind() != SketchPlugin_ConstraintLength::ID())
    return GeomPnt2dPtr();

  AttributeRefAttrPtr aLineAttr = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  if (!aLineAttr || !aLineAttr->isObject())
    return GeomPnt2dPtr();
  FeaturePtr aLine = ModelAPI_Feature::feature(aLineAttr->object());
  if (!aLine || aLine->getKind() != SketchPlugin_Line::ID())
    return GeomPnt2dPtr();

  std::shared_ptr<GeomAPI_XY> aStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLine->attribute(SketchPlugin_Line::START_ID()))->pnt()->xy();
  std::shared_ptr<GeomAPI_XY> aEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLine->attribute(SketchPlugin_Line::END_ID()))->pnt()->xy();

  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theConstraint->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  std::shared_ptr<GeomAPI_Pnt2d> aFltPnt = aFlyoutAttr->pnt();

  std::shared_ptr<GeomAPI_Dir2d> aLineDir(new GeomAPI_Dir2d(aEndPnt->decreased(aStartPnt)));

  double X = aStartPnt->x() + aFltPnt->x() * aLineDir->x() - aFltPnt->y() * aLineDir->y();
  double Y = aStartPnt->y() + aFltPnt->x() * aLineDir->y() + aFltPnt->y() * aLineDir->x();

  return GeomPnt2dPtr(new GeomAPI_Pnt2d(X, Y));
}

} // namespace SketchPlugin_Tools


// =================================================================================================
//                 namespace SketchPlugin_SegmentationTools
// =================================================================================================

void SketchPlugin_SegmentationTools::getFeaturePoints(const FeaturePtr& theFeature,
                                                      AttributePoint2DPtr& theStartPointAttr,
                                                      AttributePoint2DPtr& theEndPointAttr)
{
  std::string aFeatureKind = theFeature->getKind();
  std::string aStartAttributeName, anEndAttributeName;
  if (aFeatureKind == SketchPlugin_Line::ID()) {
    aStartAttributeName = SketchPlugin_Line::START_ID();
    anEndAttributeName = SketchPlugin_Line::END_ID();
  }
  else if (aFeatureKind == SketchPlugin_Arc::ID()) {
    aStartAttributeName = SketchPlugin_Arc::START_ID();
    anEndAttributeName = SketchPlugin_Arc::END_ID();
  }
  else if (aFeatureKind == SketchPlugin_EllipticArc::ID()) {
    aStartAttributeName = SketchPlugin_EllipticArc::START_POINT_ID();
    anEndAttributeName = SketchPlugin_EllipticArc::END_POINT_ID();
  }
  if (!aStartAttributeName.empty() && !anEndAttributeName.empty()) {
    theStartPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(aStartAttributeName));
    theEndPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(anEndAttributeName));
  }
}


void SketchPlugin_SegmentationTools::getRefAttributes(
    const FeaturePtr& theFeature,
    std::map<AttributePtr, std::list<AttributePtr> >& theRefs,
    std::list<AttributePtr>& theRefsToFeature)
{
  theRefs.clear();

  std::list<AttributePtr> aPointAttributes =
    theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::set<AttributePtr> aPointAttributesSet;

  std::list<AttributePtr>::const_iterator aPIt =
    aPointAttributes.begin(), aPLast = aPointAttributes.end();
  for (; aPIt != aPLast; aPIt++)
    aPointAttributesSet.insert(*aPIt);

  std::set<AttributePtr> aRefsAttributes = theFeature->lastResult()->data()->refsToMe();
  std::set<AttributePtr> aFRefsList = theFeature->data()->refsToMe();
  aRefsAttributes.insert(aFRefsList.begin(), aFRefsList.end());

  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsAttributes.cbegin(); aIt != aRefsAttributes.cend(); ++aIt) {
    AttributePtr anAttr = (*aIt);
    FeaturePtr anAttrFeature = ModelAPI_Feature::feature(anAttr->owner());
    if (!anAttrFeature->isMacro() && // <- skip reference from Trim or Split feature
        anAttr.get() && anAttr->attributeType() == ModelAPI_AttributeRefAttr::typeId()) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttr);
      if (!aRefAttr->isObject()) { // find attributes referenced to feature point attributes
        AttributePtr anAttrInRef = aRefAttr->attr();
        if (anAttrInRef.get() &&
            aPointAttributesSet.find(anAttrInRef) != aPointAttributesSet.end()) {
          if (theRefs.find(anAttrInRef) != theRefs.end())
            theRefs[anAttrInRef].push_back(aRefAttr);
          else {
            std::list<AttributePtr> anAttrList;
            anAttrList.push_back(aRefAttr);
            theRefs[anAttrInRef] = anAttrList;
          }
        }
      }
      else { // find attributes referenced to feature itself
        theRefsToFeature.push_back(anAttr);
      }
    }
  }
}

GeomShapePtr SketchPlugin_SegmentationTools::getSubShape(
    SketchPlugin_Feature* theFeature,
    const std::string& theObjectAttributeId,
    const std::string& thePointAttributeId,
    std::map<ObjectPtr, std::set<GeomShapePtr> >& theCashedShapes,
    std::map<ObjectPtr, GeomAlgoAPI_ShapeTools::PointToRefsMap>& theObjectToPoints)
{
  GeomShapePtr aBaseShape;

  AttributeReferencePtr anObjectAttr = theFeature->reference(theObjectAttributeId);
  ObjectPtr aBaseObject = anObjectAttr->value();
  if (!aBaseObject.get())
    return aBaseShape;

  // point on feature
  AttributePoint2DPtr aPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theFeature->attribute(thePointAttributeId));
  std::shared_ptr<GeomAPI_Pnt2d> anAttributePnt2d = aPointAttr->pnt();
  std::shared_ptr<GeomAPI_Pnt> anAttributePnt =
      theFeature->sketch()->to3D(anAttributePnt2d->x(), anAttributePnt2d->y());

  if (theCashedShapes.find(aBaseObject) == theCashedShapes.end())
    fillObjectShapes(theFeature, aBaseObject, theCashedShapes, theObjectToPoints);

  std::shared_ptr<GeomAPI_Pnt> aStartPoint;
  std::shared_ptr<GeomAPI_Pnt> aSecondPoint;
  const std::set<GeomShapePtr>& aShapes = theCashedShapes[aBaseObject];
  std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
  for (; anIt != aLast; anIt++) {
    GeomShapePtr aCurrentShape = *anIt;
    std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
    if (ModelGeomAlgo_Point2D::isPointOnEdge(aCurrentShape, anAttributePnt, aProjectedPoint)) {
      if (theFeature->getKind() == SketchPlugin_Split::ID()) {
        // for Split operation collect start and end points of the shape
        if (aCurrentShape->shapeType() == GeomAPI_Shape::EDGE) {
          std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aCurrentShape));
          aStartPoint = anEdge->firstPoint();
          aSecondPoint = anEdge->lastPoint();
        }
      }
      else
        aBaseShape = aCurrentShape;
      break;
    }
  }

  if (!aStartPoint.get() || !aSecondPoint.get())
    return aBaseShape;

  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObject);
  if (anObjectAttr->isInitialized() && aBaseFeature.get() && aPointAttr->isInitialized()) {
    ResultPtr aResult = aBaseFeature->lastResult();
    GeomShapePtr aResultShape = aResult->shape();
    std::list<std::shared_ptr<GeomAPI_Pnt> > aPoints;

    aPoints.push_back(aStartPoint);
    aPoints.push_back(aSecondPoint);

    std::set<std::shared_ptr<GeomAPI_Shape> > aSplitShapes;
    GeomAlgoAPI_ShapeTools::splitShape_p(aResultShape, aPoints, aSplitShapes);
    aBaseShape = GeomAlgoAPI_ShapeTools::findShape(aPoints, aSplitShapes);
  }
  return aBaseShape;
}

void SketchPlugin_SegmentationTools::fillObjectShapes(
    SketchPlugin_Feature* theOpFeature,
    const ObjectPtr& theObject,
    std::map<ObjectPtr, std::set<GeomShapePtr> >& theCashedShapes,
    std::map<ObjectPtr, GeomAlgoAPI_ShapeTools::PointToRefsMap>& theObjectToPoints)
{
  SketchPlugin_Sketch* aSketch = theOpFeature->sketch();

  GeomAlgoAPI_ShapeTools::PointToRefsMap aPoints;
  std::set<GeomShapePtr> aShapes;

  std::set<AttributePoint2DPtr > aRefAttributes;
  // current feature
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  std::set<ResultPtr> anEdgeShapes;
  // edges on feature
  ModelGeomAlgo_Shape::shapesOfType(aFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
  if (!anEdgeShapes.empty()) {
    GeomShapePtr aFeatureShape = (*anEdgeShapes.begin())->shape();

    // coincidences to the feature
    ModelGeomAlgo_Point2D::getPointsOfReference(aFeature, SketchPlugin_ConstraintCoincidence::ID(),
                         aRefAttributes, SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
    // layed on feature coincidences to divide it on several shapes
    std::shared_ptr<ModelAPI_Data> aData = aSketch->data();
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));

    ModelGeomAlgo_Point2D::getPointsInsideShape(aFeatureShape, aRefAttributes, aC->pnt(),
                                                  aX->dir(), aY, aPoints);

    if (theOpFeature->getKind() == SketchPlugin_Trim::ID()) {
      // collect all intersection points with other edges for Trim operation only
      std::list<FeaturePtr> aFeatures;
      for (int i = 0; i < aSketch->numberOfSubs(); i++) {
        FeaturePtr aFeature = aSketch->subFeature(i);
        if (aFeature.get() && aFeature->getKind() != SketchPlugin_Projection::ID())
          aFeatures.push_back(aFeature);
      }
      ModelGeomAlgo_Point2D::getPointsIntersectedShape(aFeature, aFeatures, aPoints);
    }

    if (!aPoints.empty())
      GeomAlgoAPI_ShapeTools::splitShape(aFeatureShape, aPoints, aShapes);
  }
  theObjectToPoints[theObject] = aPoints;
  theCashedShapes[theObject] = aShapes;
}

void SketchPlugin_SegmentationTools::updateRefAttConstraints(
    const std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
    const std::set<std::pair<AttributePtr, AttributePtr> >& theModifiedAttributes)
{
#if defined DEBUG_SPLIT || defined DEBUG_TRIM
  std::cout << "updateRefAttConstraints" << std::endl;
#endif

  std::set<std::pair<AttributePtr, AttributePtr> >::const_iterator
    anIt = theModifiedAttributes.begin(),  aLast = theModifiedAttributes.end();
  for (; anIt != aLast; anIt++) {
    AttributePtr anAttribute = anIt->first;
    AttributePtr aNewAttribute = anIt->second;

    // not found in references
    if (!aNewAttribute.get() ||
        theBaseRefAttributes.find(anAttribute) == theBaseRefAttributes.end())
      continue;
    std::list<AttributePtr> aRefAttributes = theBaseRefAttributes.at(anAttribute);
    std::list<AttributePtr>::const_iterator aRefIt = aRefAttributes.begin(),
                                            aRLast = aRefAttributes.end();

    for (; aRefIt != aRLast; aRefIt++) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefIt);
      if (aRefAttr.get()) {
        aRefAttr->setAttr(aNewAttribute);
#ifdef DEBUG_SPLIT
        FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->owner());
        std::cout << " -" << getFeatureInfo(aFeature) << std::endl;
#endif
      }
    }
  }
}

void SketchPlugin_SegmentationTools::updateFeaturesAfterOperation(
    const std::set<FeaturePtr>& theFeaturesToUpdate)
{
  std::set<FeaturePtr>::const_iterator anIt = theFeaturesToUpdate.begin(),
                                       aLast = theFeaturesToUpdate.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
    std::string aRefFeatureKind = aRefFeature->getKind();
    if (aRefFeatureKind == SketchPlugin_ConstraintLength::ID()) {
      std::shared_ptr<SketchPlugin_ConstraintLength> aLenghtFeature =
                              std::dynamic_pointer_cast<SketchPlugin_ConstraintLength>(*anIt);
      if (aLenghtFeature.get()) {
        std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
            ModelAPI_AttributeDouble>(aLenghtFeature->attribute(SketchPlugin_Constraint::VALUE()));
        double aValue;
        if (aLenghtFeature->computeLenghtValue(aValue) && aValueAttr.get())
          aValueAttr->setValue(aValue);
      }
    }
  }
}

AISObjectPtr SketchPlugin_SegmentationTools::getAISObject(
    AISObjectPtr thePrevious,
    SketchPlugin_Feature* theOpFeature,
    const std::string& thePreviewObjectAttrName,
    const std::string& thePreviewPointAttrName,
    const std::string& theSelectedObjectAttrName,
    const std::string& theSelectedPointAttrName)
{
#if defined DEBUG_SPLIT || defined DEBUG_TRIM_METHODS
  std::cout << "getAISObject: " << theOpFeature->data()->name() << std::endl;
#endif

  AISObjectPtr anAIS = thePrevious;

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  std::map<ObjectPtr, std::set<GeomShapePtr> > aCashedShapes;
  std::map<ObjectPtr, GeomAlgoAPI_ShapeTools::PointToRefsMap> aObjectToPoints;
  GeomShapePtr aPreviewShape = getSubShape(theOpFeature,
      thePreviewObjectAttrName, thePreviewPointAttrName, aCashedShapes, aObjectToPoints);
  if (aPreviewShape.get())
    aShapes.push_back(aPreviewShape);
  GeomShapePtr aSelectedShape = getSubShape(theOpFeature,
      theSelectedObjectAttrName, theSelectedPointAttrName, aCashedShapes, aObjectToPoints);
  if (aSelectedShape.get())
    aShapes.push_back(aSelectedShape);

  if (aShapes.empty())
    return AISObjectPtr();

  GeomShapePtr aBaseShape = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  if (!aBaseShape.get())
    return AISObjectPtr();

  if (aBaseShape.get()) {
    if (!anAIS)
      anAIS = AISObjectPtr(new GeomAPI_AISObject);
    anAIS->createShape(aBaseShape);

    std::vector<int> aColor;
    aColor = Config_PropManager::color("Visualization", "operation_remove_feature_color");
    double aWidth = SketchPlugin_SketchEntity::SKETCH_LINE_WIDTH();
    int aLineStyle = SketchPlugin_SketchEntity::SKETCH_LINE_STYLE();
    anAIS->setColor(aColor[0], aColor[1], aColor[2]);
    // width when there is not base object should be extened in several points
    // in order to see this preview over highlight
    anAIS->setWidth(aWidth+4);
    anAIS->setLineStyle(aLineStyle);
  }
  else
    anAIS = AISObjectPtr();
  return anAIS;
}
