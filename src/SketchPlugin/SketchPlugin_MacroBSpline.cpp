// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#include <SketchPlugin_MacroBSpline.h>

#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Tools.h>
#include <SketchPlugin_Sketch.h>

#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_EventReentrantMessage.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomDataAPI_Point2DArray.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <sstream>

// Create Point feature coincident with the B-spline pole
static FeaturePtr createAuxiliaryPole(FeaturePtr theBSpline,
                                      AttributePoint2DArrayPtr theBSplinePoles,
                                      const int thePoleIndex);
// Create segment between consequtive B-spline poles
static void createAuxiliarySegment(FeaturePtr theBSpline,
                                   AttributePoint2DArrayPtr theBSplinePoles,
                                   const int thePoleIndex1,
                                   const int thePoleIndex2);
// Create internal coincidence constraint with B-spline pole
static void createInternalConstraint(SketchPlugin_Sketch* theSketch,
                                     AttributePtr thePoint,
                                     AttributePtr theBSplinePoles,
                                     const int thePoleIndex);


SketchPlugin_MacroBSpline::SketchPlugin_MacroBSpline()
  : SketchPlugin_SketchEntity(),
    myIsPeriodic(false)
{
}

void SketchPlugin_MacroBSpline::initAttributes()
{
  data()->addAttribute(POLES_ID(), GeomDataAPI_Point2DArray::typeId());
  data()->addAttribute(WEIGHTS_ID(), ModelAPI_AttributeDoubleArray::typeId());

  data()->addAttribute(REF_POLES_ID(), ModelAPI_AttributeRefAttrList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), REF_POLES_ID());

  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());
}

void SketchPlugin_MacroBSpline::execute()
{
  FeaturePtr aBSpline = createBSplineFeature();

  std::list<FeaturePtr> aControlPoles;
  createControlPolygon(aBSpline, aControlPoles);
  constraintsForPoles(aControlPoles);

  // message to init reentrant operation
  static Events_ID anId = ModelAPI_EventReentrantMessage::eventId();
  ReentrantMessagePtr aMessage(new ModelAPI_EventReentrantMessage(anId, this));
  // set here the last pole to make coincidence with the start point of the next B-spline curve
  aMessage->setCreatedFeature(aControlPoles.back());
  Events_Loop::loop()->send(aMessage);
}

// LCOV_EXCL_START
std::string SketchPlugin_MacroBSpline::processEvent(
                                              const std::shared_ptr<Events_Message>& theMessage)
{
  ReentrantMessagePtr aReentrantMessage =
      std::dynamic_pointer_cast<ModelAPI_EventReentrantMessage>(theMessage);
  if (aReentrantMessage) {
    FeaturePtr aCreatedFeature = aReentrantMessage->createdFeature();
    ObjectPtr anObject = aReentrantMessage->selectedObject();
    AttributePtr anAttribute = aReentrantMessage->selectedAttribute();
    std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = aReentrantMessage->clickedPoint();

    if (aClickedPoint) {
      // fill points list (it consists of 2 points to make editable the second one)
      AttributePoint2DArrayPtr aPointArrayAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(attribute(POLES_ID()));
      aPointArrayAttr->setSize(2);
      aPointArrayAttr->setPnt(0, aClickedPoint);
      aPointArrayAttr->setPnt(1, aClickedPoint);

      // fill weights
      AttributeDoubleArrayPtr aWeightsArrayAttr = data()->realArray(WEIGHTS_ID());
      aWeightsArrayAttr->setSize(2);
      aWeightsArrayAttr->setValue(0, 1.0);
      aWeightsArrayAttr->setValue(1, 1.0);

      // fill reference attribute
      AttributeRefAttrListPtr aRefAttrList =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(attribute(REF_POLES_ID()));
      if (anAttribute) {
        if (!anAttribute->owner() || !anAttribute->owner()->data()->isValid()) {
          if (aCreatedFeature && anAttribute->id() == SketchPlugin_Point::COORD_ID())
            anAttribute = aCreatedFeature->attribute(SketchPlugin_Point::COORD_ID());
        }
        aRefAttrList->append(anAttribute);
      }
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return std::string();
}
// LCOV_EXCL_STOP

FeaturePtr SketchPlugin_MacroBSpline::createBSplineFeature()
{
  FeaturePtr aBSpline = sketch()->addFeature(SketchPlugin_BSpline::ID());

  AttributePoint2DArrayPtr aPoles = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      aBSpline->attribute(SketchPlugin_BSpline::POLES_ID()));
  AttributePoint2DArrayPtr aPolesMacro =
      std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(attribute(POLES_ID()));
  aPoles->assign(aPolesMacro);

  AttributeDoubleArrayPtr aWeights =
      aBSpline->data()->realArray(SketchPlugin_BSpline::WEIGHTS_ID());
  AttributeDoubleArrayPtr aWeightsMacro = data()->realArray(WEIGHTS_ID());
  int aSize = aWeightsMacro->size();
  aWeights->setSize(aSize);
  for (int index = 0; index < aSize; ++index)
    aWeights->setValue(index, aWeightsMacro->value(index));

  aBSpline->boolean(SketchPlugin_BSpline::AUXILIARY_ID())->setValue(
      boolean(AUXILIARY_ID())->value());

  aBSpline->execute();

  return aBSpline;
}

void SketchPlugin_MacroBSpline::createControlPolygon(FeaturePtr theBSpline,
                                                     std::list<FeaturePtr>& thePoles)
{
  AttributePoint2DArrayPtr aPoles = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      theBSpline->attribute(SketchPlugin_BSpline::POLES_ID()));
  int aSize = aPoles->size();
  // poles
  for (int index = 0; index < aSize; ++index)
    thePoles.push_back(createAuxiliaryPole(theBSpline, aPoles, index));
  // segments
  for (int index = 1; index < aSize; ++index)
    createAuxiliarySegment(theBSpline, aPoles, index - 1, index);
}

void SketchPlugin_MacroBSpline::constraintsForPoles(const std::list<FeaturePtr>& thePoles)
{
  AttributeRefAttrListPtr aRefAttrList = data()->refattrlist(REF_POLES_ID());
  std::list<std::pair<ObjectPtr, AttributePtr> > aList;
  if (aRefAttrList)
    aList = aRefAttrList->list();

  SketchPlugin_Sketch* aSketch = sketch();

  std::list<std::pair<ObjectPtr, AttributePtr> >::iterator aLIt = aList.begin();
  std::list<FeaturePtr>::const_iterator aPIt = thePoles.begin();
  for (; aLIt != aList.end() && aPIt != thePoles.end(); ++aPIt, ++aLIt) {
    // firstly, check the attribute (in this case the object will be not empty too)
    if (aLIt->second) {
      SketchPlugin_Tools::createConstraintAttrAttr(aSketch,
          SketchPlugin_ConstraintCoincidence::ID(),
          (*aPIt)->attribute(SketchPlugin_Point::COORD_ID()), aLIt->second);
    }
    // now add coincidence with the result
    else if (aLIt->first) {
      SketchPlugin_Tools::createConstraintAttrObject(aSketch,
          SketchPlugin_ConstraintCoincidence::ID(),
          (*aPIt)->attribute(SketchPlugin_Point::COORD_ID()), aLIt->first);
    }
  }
}

AISObjectPtr SketchPlugin_MacroBSpline::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return AISObjectPtr();

  AttributePoint2DArrayPtr aPolesArray =
      std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(attribute(POLES_ID()));
  AttributeDoubleArrayPtr aWeightsArray = data()->realArray(WEIGHTS_ID());

  std::list<GeomShapePtr> aShapes;

  // convert poles to 3D and collect weights
  std::vector<GeomPointPtr> aPoles3D;
  aPoles3D.reserve(aPolesArray->size());
  std::vector<double> aWeights;
  aWeights.reserve(aWeightsArray->size());
  for (int anIndex = 0; anIndex < aPolesArray->size(); ++anIndex) {
    double aWeight = aWeightsArray->value(anIndex);
    if (aWeight < 1.e-10)
      continue; // skip poles with zero weights

    aWeights.push_back(aWeight);

    GeomPnt2dPtr aPole = aPolesArray->pnt(anIndex);
    GeomPointPtr aPole3D = aSketch->to3D(aPole->x(), aPole->y());
    aPoles3D.push_back(aPole3D);
    aShapes.push_back(GeomAlgoAPI_PointBuilder::vertex(aPole3D));
  }

  // create result non-periodic B-spline curve
  GeomShapePtr anEdge = GeomAlgoAPI_EdgeBuilder::bspline(aPoles3D, aWeights, false);
  if (!anEdge)
    return AISObjectPtr();

  aShapes.push_back(anEdge);
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS.reset(new GeomAPI_AISObject());
  anAIS->createShape(aCompound);

  // Modify attributes
  SketchPlugin_Tools::customizeFeaturePrs(anAIS, boolean(AUXILIARY_ID())->value());

  return anAIS;
}



// ==========================     Auxiliary functions    ===========================================

FeaturePtr createAuxiliaryPole(FeaturePtr theBSpline,
                               AttributePoint2DArrayPtr theBSplinePoles,
                               const int thePoleIndex)
{
  SketchPlugin_Sketch* aSketch =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theBSpline)->sketch();

  // create child point equal to the B-spline's pole
  FeaturePtr aPointFeature = aSketch->addFeature(SketchPlugin_Point::ID());
  aPointFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
  aPointFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(theBSpline);

  GeomPnt2dPtr aPole = theBSplinePoles->pnt(thePoleIndex);

  AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aPointFeature->attribute(SketchPlugin_Point::COORD_ID()));
  aCoord->setValue(aPole);

  aPointFeature->execute();

  std::ostringstream aName;
  aName << theBSpline->name() << "_" << theBSplinePoles->id() << "_" << thePoleIndex;
  aPointFeature->data()->setName(aName.str());
  aPointFeature->lastResult()->data()->setName(aName.str());

  // internal constraint to keep position of the point
  createInternalConstraint(aSketch, aCoord, theBSplinePoles, thePoleIndex);

  return aPointFeature;
}

void createAuxiliarySegment(FeaturePtr theBSpline,
                            AttributePoint2DArrayPtr theBSplinePoles,
                            const int thePoleIndex1,
                            const int thePoleIndex2)
{
  SketchPlugin_Sketch* aSketch =
    std::dynamic_pointer_cast<SketchPlugin_Feature>(theBSpline)->sketch();

  // create child segment between B-spline poles
  FeaturePtr aLineFeature = aSketch->addFeature(SketchPlugin_Line::ID());
  aLineFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
  aLineFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(theBSpline);

  AttributePoint2DPtr aLineStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aLineFeature->attribute(SketchPlugin_Line::START_ID()));
  aLineStart->setValue(theBSplinePoles->pnt(thePoleIndex1));

  AttributePoint2DPtr aLineEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aLineFeature->attribute(SketchPlugin_Line::END_ID()));
  aLineEnd->setValue(theBSplinePoles->pnt(thePoleIndex2));

  aLineFeature->execute();

  std::ostringstream aName;
  aName << theBSpline->name() << "_segment_" << thePoleIndex1 << "_" << thePoleIndex2;
  aLineFeature->data()->setName(aName.str());
  aLineFeature->lastResult()->data()->setName(aName.str());

  // internal constraints to keep the segment position
  createInternalConstraint(aSketch, aLineStart, theBSplinePoles, thePoleIndex1);
  createInternalConstraint(aSketch, aLineEnd, theBSplinePoles, thePoleIndex2);
}

void createInternalConstraint(SketchPlugin_Sketch* theSketch,
                              AttributePtr thePoint,
                              AttributePtr theBSplinePoles,
                              const int thePoleIndex)
{
  std::shared_ptr<SketchPlugin_ConstraintCoincidenceInternal> aConstraint =
      std::dynamic_pointer_cast<SketchPlugin_ConstraintCoincidenceInternal>(
      theSketch->addFeature(SketchPlugin_ConstraintCoincidenceInternal::ID()));
  aConstraint->refattr(SketchPlugin_Constraint::ENTITY_A())->setAttr(thePoint);
  aConstraint->refattr(SketchPlugin_Constraint::ENTITY_B())->setAttr(theBSplinePoles);
  aConstraint->integer(SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_B())
      ->setValue(thePoleIndex);
}
