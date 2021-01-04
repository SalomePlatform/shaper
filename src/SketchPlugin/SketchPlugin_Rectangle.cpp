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
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

#include <SketchPlugin_Line.h>
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
  data()->addAttribute(START_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
  data()->addAttribute(LINES_LIST_ID(), ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), LINES_LIST_ID());
  data()->addAttribute(ISHV_LIST_ID(), ModelAPI_AttributeIntArray::typeId());

  data()->addAttribute(NOT_TO_DUMP_LIST_ID(), ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), NOT_TO_DUMP_LIST_ID());

  AttributeIntArrayPtr isHVList = intArray(ISHV_LIST_ID());
  isHVList->setSize(4, false);
  for(int i = 0; i< 4;)
    isHVList->setValue(i++, 0, false);
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

  double xMin = std::min(aStartPoint->x(),  aEndPoint->x());
  double xMax = std::max(aStartPoint->x(),  aEndPoint->x());
  double yMin = std::min(aStartPoint->y(),  aEndPoint->y());
  double yMax = std::max(aStartPoint->y(),  aEndPoint->y());
  std::vector<double> aX = {xMin, xMax, xMax, xMin};
  std::vector<double> aY = {yMin, yMin, yMax, yMax};

  bool anAuxiliary = data()->boolean(AUXILIARY_ID())->value();

  /// Update coordinates of rectangle lines
  for(unsigned  i = 0; i < aNbLines; i++)
  {
    FeaturePtr aLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(i));
    std::shared_ptr<GeomDataAPI_Point2D> aLineStart =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aLine->attribute(SketchPlugin_Line::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aLineEnd =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aLine->attribute(SketchPlugin_Line::END_ID()));
       aLineStart->setValue(aX[i], aY[i]);
    aLineEnd->setValue(aX[(i+1)%4], aY[(i+1)%4]);
    aLine->data()->boolean(AUXILIARY_ID())->setValue(anAuxiliary);
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
    /// Create constraints to keep the rectangle
    for( unsigned i = 0; i < aNbLines; i++)
    {
      FeaturePtr  aLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(i));
      /// connect neighbor lines by coincidence
      unsigned iPrev = (i+3)%4;
      FeaturePtr aPrevLine = std::dynamic_pointer_cast<ModelAPI_Feature>(aLinesList->object(iPrev));
      FeaturePtr aCoincidence = aSketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
      aNotToDumpList->append(aCoincidence);
      AttributeRefAttrPtr aRefAttrA = aCoincidence->refattr(SketchPlugin_ConstraintCoincidence::ENTITY_A());
      AttributeRefAttrPtr aRefAttrB = aCoincidence->refattr(SketchPlugin_ConstraintCoincidence::ENTITY_B());
      aRefAttrA->setAttr(aPrevLine->attribute(SketchPlugin_Line::END_ID()));
      aRefAttrB->setAttr(aLine->attribute(SketchPlugin_Line::START_ID()));
    }

    /// Update coordinates of created lines
    updateLines();
  }

  /// Add horizontal and vertical constraint for the lines which already have result
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
    AttributeRefAttrPtr aRefAttrA = aHVConstraint->refattr(SketchPlugin_ConstraintCoincidence::ENTITY_A());
    aRefAttrA->setObject(aLine->lastResult());
    isHVList->setValue(i, 1, false);
  }

  double aDiag = std::pow(aStartAttr->x() - aEndAttr->x(), 2.0);
  aDiag += std::pow(aStartAttr->y() - aEndAttr->y(), 2.0);
  aDiag = std::sqrt(aDiag);
  if(aDiag < tolerance) {
    return;
  }

  // store results.

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

  aRectangleShape = aSubs.empty() ? GeomShapePtr() : GeomAlgoAPI_CompoundBuilder::compound(aSubs);

  std::shared_ptr<ModelAPI_ResultConstruction> aResult = document()->createConstruction(data(), 0);
  aResult->setShape(aRectangleShape);
  aResult->setIsInHistory(false);
  setResult(aResult, 1);
}


bool SketchPlugin_Rectangle::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_Rectangle::attributeChanged(const std::string& theID)
{
  if (theID == START_ID() || theID == END_ID())
  {
    AttributeRefListPtr aLinesList = reflist(LINES_LIST_ID());
    AttributeRefListPtr aNotToDumpList =  reflist(NOT_TO_DUMP_LIST_ID());
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
    }

    std::shared_ptr<GeomDataAPI_Point2D> aStartPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END_ID()));

    if (aStartPoint->isInitialized() && aEndPoint->isInitialized())
      updateLines();    
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
