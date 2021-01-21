// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "SketchPlugin_Rectangle.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Tools.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintVertical.h>

#include <cmath>


const double tolerance = 1e-7;


SketchPlugin_Rectangle::SketchPlugin_Rectangle()
  : SketchPlugin_SketchEntity()
{
}


void SketchPlugin_Rectangle::initDerivedClassAttributes()
{
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(START_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CENTER_ID());
  data()->addAttribute(CENTER_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CENTER_REF_ID());
  data()->addAttribute(LINES_LIST_ID(), ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), LINES_LIST_ID());
  data()->addAttribute(DIAGONAL_LIST_ID(), ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), DIAGONAL_LIST_ID());
  data()->addAttribute(ISHV_LIST_ID(), ModelAPI_AttributeIntArray::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ISHV_LIST_ID());
  data()->addAttribute(NOT_TO_DUMP_LIST_ID(), ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), NOT_TO_DUMP_LIST_ID());
}

namespace {
  static const std::pair<unsigned, std::string> cornerToDiagonalLinePoints[4]
  = {
    {0, SketchPlugin_Line::START_ID()},
    {1, SketchPlugin_Line::START_ID()},
    {0, SketchPlugin_Line::END_ID()},
    {1, SketchPlugin_Line::END_ID()}
  };
}

void SketchPlugin_Rectangle::updateLines()
{
  // Retrieving list of already created lines
  AttributeRefListPtr aLinesList = reflist(LINES_LIST_ID());
  unsigned  aNbLines = aLinesList->size();
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END_ID()));

  double aXStart = aStartPoint->x();
  double aYStart = aStartPoint->y();
  double aXEnd = aEndPoint->x();
  double aYEnd = aEndPoint->y();

  std::vector<double> aX = {aXStart, aXStart, aXEnd, aXEnd};
  std::vector<double> aY = {aYStart, aYEnd, aYEnd, aYStart};

  bool anAuxiliary = data()->boolean(AUXILIARY_ID())->value();
  AttributeRefListPtr aDiagonalList =  reflist(DIAGONAL_LIST_ID());


  /// Update coordinates of rectangle lines
  for(unsigned  i = 0; i < aNbLines; i++)
  {
    FeaturePtr aLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(i));
    std::shared_ptr<GeomDataAPI_Point2D> aLineStart =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aLine->attribute(SketchPlugin_Line::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aLineEnd =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aLine->attribute(SketchPlugin_Line::END_ID()));
    aLineStart->setValue(aX[(i+3)%4], aY[(i+3)%4]);
    aLineEnd->setValue(aX[i], aY[i]);
    aLine->data()->boolean(AUXILIARY_ID())->setValue(anAuxiliary);
    /// Cooordinates of diagonals
    if(aDiagonalList->size())
    {
      auto aDiagonalPoint = cornerToDiagonalLinePoints[i];
      FeaturePtr aDiagonal = std::dynamic_pointer_cast<ModelAPI_Feature>(aDiagonalList->object(aDiagonalPoint.first));
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aDiagonal->attribute(aDiagonalPoint.second))->setValue(aX[(i+3)%4], aY[(i+3)%4]);
    }
  }
}

void SketchPlugin_Rectangle::updateStartPoint()
{
  /// Retrieving list of already created lines
  AttributeRefListPtr aLinesList = reflist(LINES_LIST_ID());
  unsigned  aNbLines = aLinesList->size();
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START_ID()));

  double aXStart = aStartPoint->x();
  double aYStart = aStartPoint->y();

  /// Update coordinates of rectangle lines
  for(unsigned  i = 0; i < aNbLines; i++)
  {
    FeaturePtr aLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(i));
    std::shared_ptr<GeomDataAPI_Point2D> aLineStart =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aLine->attribute(SketchPlugin_Line::END_ID()));
    aLineStart->setValue(aXStart, aYStart);
  }
}

void SketchPlugin_Rectangle::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return;
  }

  // Compute a Rectangle in 3D view.
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START_ID()));

  std::shared_ptr<GeomDataAPI_Point2D> aEndAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END_ID()));

  if(!aStartAttr->isInitialized() || !aEndAttr->isInitialized()) {
    return;
  }

  AttributeRefListPtr aLinesList =  reflist(LINES_LIST_ID());
  unsigned aNbLines = aLinesList->size();
  AttributeIntArrayPtr isHVList = intArray(ISHV_LIST_ID());
  AttributeRefListPtr aNotToDumpList = reflist(NOT_TO_DUMP_LIST_ID());
  AttributeRefListPtr aDiagonalList =  reflist(DIAGONAL_LIST_ID());

  if(aNbLines == 1)
  {
    /// Create 1-4 lines to compose the rectangle
    for( unsigned i = 0; i < 3; i++)
    {
      FeaturePtr aLine = aSketch->addFeature(SketchPlugin_Line::ID());
      aLinesList->append(aLine);
      aNotToDumpList->append(aLine);
    }

    updateLines();

    aNbLines = aLinesList->size();
    FeaturePtr aCenterPointFeature;

    if(aDiagonalList->size())
    {
      /// compute diagonals intersection point
      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));

      aCenterPointFeature = aSketch->addFeature(SketchPlugin_Point::ID());
      aNotToDumpList->append(aCenterPointFeature);
      AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
            aCenterPointFeature->attribute(SketchPlugin_Point::COORD_ID()));
      aCoord->setValue(aCenterAttr->x(), aCenterAttr->y());
      aCenterPointFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
      refattr(CENTER_REF_ID())->setObject(aCenterPointFeature);

      for(int i = 0; i < 2; i++)
      {
        FeaturePtr aDiagonal = std::dynamic_pointer_cast<ModelAPI_Feature>(aDiagonalList->object(i));
        FeaturePtr aConstraint = SketchPlugin_Tools::createConstraintAttrObject(aSketch, SketchPlugin_ConstraintCoincidence::ID(),
                                                                                aCoord, aDiagonal);
        aNotToDumpList->append(aConstraint);
      }
    }

    /// Create constraints to keep the rectangle
    for( unsigned i = 0; i < aNbLines; i++)
    {
      FeaturePtr  aLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(i));
      /// connect neighbor lines by coincidence
      unsigned iPrev = (i+3)%4;
      FeaturePtr aPrevLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(iPrev));
      FeaturePtr aConstraint = SketchPlugin_Tools::createConstraintAttrAttr(aSketch, SketchPlugin_ConstraintCoincidence::ID(),
                                                                            aPrevLine->attribute(SketchPlugin_Line::END_ID()),
                                                                            aLine->attribute(SketchPlugin_Line::START_ID()));
      aNotToDumpList->append(aConstraint);

      /// case of  rectangle created from its center
      if(aDiagonalList->size())
      {
        auto aDiagonalPoint = cornerToDiagonalLinePoints[i];
        FeaturePtr aDiagonal = std::dynamic_pointer_cast<ModelAPI_Feature>(aDiagonalList->object(aDiagonalPoint.first));
        FeaturePtr aConstraint = SketchPlugin_Tools::createConstraintAttrAttr(aSketch, SketchPlugin_ConstraintCoincidence::ID(),
                                                                              aDiagonal->attribute(aDiagonalPoint.second),
                                                                              aLine->attribute(SketchPlugin_Line::START_ID()));
        aNotToDumpList->append(aConstraint);
      }
    }
    /// Update coordinates of created lines
    updateLines();
  }

  /// Add horizontal and vertical constraint for the lines which already have result
  if(isHVList->size() == 0)
  {
    isHVList->setSize(4, false);
    for(int i = 0; i< 4;)
      isHVList->setValue(i++, 0, false);
  }

  for(unsigned i = 0; i< aNbLines; i++)
  {
    if(isHVList->value(i))
      continue;
    FeaturePtr aLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(i));
    ResultPtr aLineResult = aLine->lastResult();
    if(!aLineResult.get())
      continue;
    std::string aHVName = SketchPlugin_ConstraintHorizontal::ID();
    if(i%2)
      aHVName = SketchPlugin_ConstraintVertical::ID();
    FeaturePtr aHVConstraint = aSketch->addFeature(aHVName);
    aNotToDumpList->append(aHVConstraint);
    AttributeRefAttrPtr aRefAttrA = aHVConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
    aRefAttrA->setObject(aLine->lastResult());
    isHVList->setValue(i, 1, false);
  }

  double aDiag = std::pow(aStartAttr->x() - aEndAttr->x(), 2.0);
  aDiag += std::pow(aStartAttr->y() - aEndAttr->y(), 2.0);
  aDiag = std::sqrt(aDiag);
  if(aDiag < tolerance) {
    return;
  }

  /// store results.
  GeomShapePtr aRectangleShape;
  ListOfShape aSubs;

  for(unsigned i = 0; i< aNbLines; i++)
  {
    FeaturePtr aLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(i));
    ResultPtr aLineResult = aLine->lastResult();
    if(!aLineResult.get())
      continue;
    aSubs.push_back(aLineResult->shape());
  }

  for(int i = 0; i< aDiagonalList->size(); i++)
  {
    FeaturePtr aDiagonal = std::dynamic_pointer_cast<ModelAPI_Feature>(aDiagonalList->object(i));
    ResultPtr aDiagonalResult = aDiagonal->lastResult();
    if(!aDiagonalResult.get())
      continue;
    aSubs.push_back(aDiagonalResult->shape());
  }

  FeaturePtr aCenterPointFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(refattr(CENTER_REF_ID())->object());
  if(aCenterPointFeature)
  {
    ResultPtr aCenterResult = aCenterPointFeature->lastResult();
    if(aCenterResult.get())
      aSubs.push_back(aCenterResult->shape());
  }

  aRectangleShape = aSubs.empty() ? GeomShapePtr() : GeomAlgoAPI_CompoundBuilder::compound(aSubs);
  std::shared_ptr<ModelAPI_ResultConstruction> aResult = document()->createConstruction(data(), 0);
  aResult->setShape(aRectangleShape);
  aResult->setIsInHistory(false);
  setResult(aResult, 1);
}

void SketchPlugin_Rectangle::attributeChanged(const std::string& theID)
{
  if (theID == START_ID() || theID == END_ID())
  {
    AttributeRefListPtr aLinesList = reflist(LINES_LIST_ID());
    AttributeRefListPtr aNotToDumpList = reflist(NOT_TO_DUMP_LIST_ID());
    unsigned  aNbLines = aLinesList->size();
    if(aNbLines == 0)
    {
      SketchPlugin_Sketch* aSketch = sketch();
      if(!aSketch) {
        return;
      }
      /// Create first line to be able to create a coincidence with selected point/feature
      FeaturePtr aLine = aSketch->addFeature(SketchPlugin_Line::ID());
      aLinesList->append(aLine);
      aNotToDumpList->append(aLine);

      /// if rectangle has center, add 2 iagonals
      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
      if(aCenterAttr->isInitialized())
      {
        AttributeRefListPtr aDiagonalList = reflist(DIAGONAL_LIST_ID());
        for(int i = 0; i < 2; i++)
        {
          FeaturePtr aDiagonalLine = aSketch->addFeature(SketchPlugin_Line::ID());
          aDiagonalLine->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
          aDiagonalList->append(aDiagonalLine);
          aNotToDumpList->append(aDiagonalLine);
        }
      }
    }
    std::shared_ptr<GeomDataAPI_Point2D> aStartPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END_ID()));

    if (aStartPoint->isInitialized() && aEndPoint->isInitialized())
      updateLines();
    else
      updateStartPoint();
  }

  if (theID == AUXILIARY_ID())
  {
    bool anAuxiliary = data()->boolean(AUXILIARY_ID())->value();
    AttributeRefListPtr aLinesList = reflist(LINES_LIST_ID());
    unsigned aNbLines = aLinesList->size();

    /// Update coordinates of rectangle lines
    for(unsigned  i = 0; i < aNbLines; i++)
    {
      FeaturePtr aLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(i));
      aLine->data()->boolean(AUXILIARY_ID())->setValue(anAuxiliary);
    }
  }
}
