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

#include "SketchPlugin_MacroRectangle.h"
#include "SketchPlugin_Rectangle.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Events.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <GeomAPI_Pnt2d.h>

#include <cmath>

const double tolerance = 1e-7;


SketchPlugin_MacroRectangle::SketchPlugin_MacroRectangle()
  : SketchPlugin_SketchEntity(), myHasCenterPoint(false)
{  
}

void SketchPlugin_MacroRectangle::initAttributes()
{
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(START1_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END1_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END2_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(RECTANGLE_TYPE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EDIT_RECTANGLE_TYPE_ID(), ModelAPI_AttributeString::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EDIT_RECTANGLE_TYPE_ID());

  string(EDIT_RECTANGLE_TYPE_ID())->setValue("");
}

void SketchPlugin_MacroRectangle::endPoint()
{
  std::shared_ptr<GeomDataAPI_Point2D> aEndPoint;
  if(string(RECTANGLE_TYPE_ID())->value() == START_END_POINT_TYPE_ID())
    aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END1_ID()));
  else
    aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END2_ID()));
  if(aEndPoint->isInitialized())
    myEndPoint = std::make_shared<GeomAPI_Pnt2d>(aEndPoint->x(), aEndPoint->y());
  else
    myEndPoint.reset();
}

void SketchPlugin_MacroRectangle::startPoint()
{
  if(string(RECTANGLE_TYPE_ID())->value() == START_END_POINT_TYPE_ID())
  {
    std::shared_ptr<GeomDataAPI_Point2D> aStartPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(START1_ID()));
    if(aStartPoint->isInitialized())
      myStartPoint = std::make_shared<GeomAPI_Pnt2d>(aStartPoint->x(), aStartPoint->y());
    else
      myStartPoint.reset();
  }
  else
  {
    /// Compute end point as the symmetric of start point w.r.t. center
    std::shared_ptr<GeomDataAPI_Point2D> aEndPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(END2_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aCenterPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));

    if(aCenterPoint->isInitialized())
    {
      myCenterPoint = std::make_shared<GeomAPI_Pnt2d>(aCenterPoint->x(), aCenterPoint->y());
      myHasCenterPoint = true;
    }
    double xStart = 2.0*aCenterPoint->x() - aEndPoint->x();
    double yStart = 2.0*aCenterPoint->y() - aEndPoint->y();

    if(aEndPoint->isInitialized() && aCenterPoint->isInitialized())
      myStartPoint =  std::make_shared<GeomAPI_Pnt2d>(xStart, yStart);
    else
      myStartPoint.reset();
  }
}


void SketchPlugin_MacroRectangle::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if(!myStartPoint || !myEndPoint || !aSketch) {
    return ;
  }
  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  /// create a rectangle sketch
  FeaturePtr myRectangleFeature = aSketch->addFeature(SketchPlugin_Rectangle::ID());
  if(!myRectangleFeature)
    return;

  if(myHasCenterPoint){
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          myRectangleFeature->attribute(SketchPlugin_Rectangle::CENTER_ID()))->setValue(myCenterPoint->x(),
                                                                                       myCenterPoint->y());
  }

  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        myRectangleFeature->attribute(SketchPlugin_Rectangle::START_ID()))->setValue(myStartPoint->x(),
                                                                                     myStartPoint->y());
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        myRectangleFeature->attribute(SketchPlugin_Rectangle::END_ID()))->setValue(myEndPoint->x(),
                                                                                   myEndPoint->y());

  myRectangleFeature->boolean(SketchPlugin_Rectangle::AUXILIARY_ID())
      ->setValue(boolean(AUXILIARY_ID())->value());
  myRectangleFeature->execute();

  /// Send events to update the sub-features by the solver.
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);  
}

void SketchPlugin_MacroRectangle::attributeChanged(const std::string& theID)
{
  if(theID == RECTANGLE_TYPE_ID()) {
    SketchPlugin_Tools::resetAttribute(this, START1_ID());
    SketchPlugin_Tools::resetAttribute(this, END1_ID());
    SketchPlugin_Tools::resetAttribute(this, CENTER_ID());
    SketchPlugin_Tools::resetAttribute(this, END2_ID());
  }
  else if (theID == START1_ID() || theID == END1_ID() ||
           theID == END2_ID() || theID == CENTER_ID())
  {
    // update points
    startPoint();
    endPoint();
  }
  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  data()->blockSendAttributeUpdated(aWasBlocked, false);
}

AISObjectPtr SketchPlugin_MacroRectangle::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();

  if(!aSketch || !myEndPoint || ! myStartPoint)
    return AISObjectPtr();

  std::vector<double> aX = {myStartPoint->x(), myStartPoint->x(), myEndPoint->x(), myEndPoint->x()};
  std::vector<double> aY = {myStartPoint->y(), myEndPoint->y(), myEndPoint->y(), myStartPoint->y()};

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  /// Update coordinates of rectangle lines

  std::set<int> createdPointIndex;
  for(unsigned  i = 0; i < 4; i++)
  {
    std::shared_ptr<GeomAPI_Pnt> theStart(aSketch->to3D(aX[(i+3)%4], aY[(i+3)%4]));
    std::shared_ptr<GeomAPI_Pnt> theEnd(aSketch->to3D(aX[i], aY[i]));
    GeomShapePtr aLine = GeomAlgoAPI_EdgeBuilder::line(theStart, theEnd);

    if(aLine)
    {
      aShapes.push_back(aLine);
      if(createdPointIndex.insert(i).second){
        GeomShapePtr aPointShape = GeomAlgoAPI_PointBuilder::vertex(theStart);
        aShapes.push_back(aPointShape);
      }
      if(createdPointIndex.insert((i+1)%4).second){
        GeomShapePtr aPointShape = GeomAlgoAPI_PointBuilder::vertex(theEnd);
        aShapes.push_back(aPointShape);
      }
    }
  }

  if(string(RECTANGLE_TYPE_ID())->value() == CENTER_END_POINT_TYPE_ID()){
    /// draw  a line center->end
    std::shared_ptr<GeomDataAPI_Point2D> aCenterPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));

    std::shared_ptr<GeomAPI_Pnt> theEnd(aSketch->to3D(myEndPoint->x(), myEndPoint->y()));
    std::shared_ptr<GeomAPI_Pnt> theStart(aSketch->to3D(aCenterPoint->x(), aCenterPoint->y()));
    GeomShapePtr aLine = GeomAlgoAPI_EdgeBuilder::line(theStart, theEnd);
    if(aLine)
      aShapes.push_back(aLine);
  }

  // Compute a rectangle in 3D view.

  std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  AISObjectPtr anAIS = thePrevious;
  if(!anAIS.get()) {
    anAIS.reset(new GeomAPI_AISObject());
  }
  anAIS->createShape(aCompound);

  // Modify attributes
  SketchPlugin_Tools::customizeFeaturePrs(anAIS, boolean(AUXILIARY_ID())->value());

  return anAIS;
}

