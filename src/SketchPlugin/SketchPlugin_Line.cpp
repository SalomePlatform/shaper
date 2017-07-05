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

#include "SketchPlugin_Line.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_ConstraintCoincidence.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_EventReentrantMessage.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomDataAPI_Point2D.h>

SketchPlugin_Line::SketchPlugin_Line()
    : SketchPlugin_SketchEntity()
{}

void SketchPlugin_Line::initAttributes()
{
  SketchPlugin_SketchEntity::initAttributes();
  /// new attributes should be added to end of the feature in order to provide
  /// correct attribute values in previous saved studies
  data()->addAttribute(LENGTH_ID(), ModelAPI_AttributeDouble::typeId());
}

void SketchPlugin_Line::initDerivedClassAttributes()
{
  data()->addAttribute(START_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_Line::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // compute a start point in 3D view
    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(START_ID()));
    // compute an end point in 3D view
    std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(END_ID()));
    if (aStartAttr->isInitialized() && anEndAttr->isInitialized()) {
      std::shared_ptr<GeomAPI_Pnt> aStart(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));
      std::shared_ptr<GeomAPI_Pnt> anEnd(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));
      //std::cout<<"Execute line "<<aStart->x()<<" "<<aStart->y()<<" "<<aStart->z()<<" - "
      //  <<anEnd->x()<<" "<<anEnd->y()<<" "<<anEnd->z()<<std::endl;
      // make linear edge
      std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);
      // store the result
      std::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(
          data());
      aConstr->setShape(anEdge);
      aConstr->setIsInHistory(false);
      setResult(aConstr);

      static Events_ID anId = ModelAPI_EventReentrantMessage::eventId();
      std::shared_ptr<ModelAPI_EventReentrantMessage> aMessage = std::shared_ptr
          <ModelAPI_EventReentrantMessage>(new ModelAPI_EventReentrantMessage(anId, this));
      aMessage->setCreatedFeature(ModelAPI_Feature::feature(
                                  data()->attribute(START_ID())->owner()));
      Events_Loop::loop()->send(aMessage);
    }
  }
}

std::string SketchPlugin_Line::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  std::string aFilledAttributeName;

  std::shared_ptr<ModelAPI_EventReentrantMessage> aReentrantMessage =
        std::dynamic_pointer_cast<ModelAPI_EventReentrantMessage>(theMessage);
  if (aReentrantMessage.get()) {
    FeaturePtr aCreatedFeature = aReentrantMessage->createdFeature();

    // Initialize new line with first point equal to end of previous
    std::shared_ptr<ModelAPI_Data> aSFData = aCreatedFeature->data();
    std::shared_ptr<GeomDataAPI_Point2D> aSPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                 aSFData->attribute(SketchPlugin_Line::END_ID()));
    std::shared_ptr<ModelAPI_Data> aNFData = data();
    std::shared_ptr<GeomDataAPI_Point2D> aNPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                 aNFData->attribute(SketchPlugin_Line::START_ID()));
    aNPoint->setValue(aSPoint->x(), aSPoint->y());
    SketchPlugin_ConstraintCoincidence::createCoincidenceFeature(sketch(), aSPoint, aNPoint);
  }
  return aFilledAttributeName;
}

double SketchPlugin_Line::distanceToPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  double aDelta = 0;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(END_ID()));

  GeomAPI_Lin2d aLin2d(aPoint1->x(), aPoint1->y(), aPoint2->x(), aPoint2->y());

  if (false/*projection*/) {  // TODO: if it has not been necessary, remove this block
    std::shared_ptr<GeomAPI_Pnt2d> aResult = aLin2d.project(thePoint);
    aDelta = aResult->distance(thePoint);
  } else {  // distance
    aDelta = aLin2d.distance(thePoint);
  }

  return aDelta;
}

const std::string& SketchPlugin_Line::getKind()
{
  static std::string MY_KIND = SketchPlugin_Line::ID();
  return MY_KIND;
}

bool SketchPlugin_Line::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_Line::attributeChanged(const std::string& theID) {
  // the second condition for unability to move external segments anywhere
  // isCopy() is checked temporary for case when copied lines stored external id state
  // to be removed after debug
  if ((theID == EXTERNAL_ID() || isFixed()) && !isCopy()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
    if (!aSelection) {
      // empty shape in selection shows that the shape is equal to context
      ResultPtr anExtRes = selection(EXTERNAL_ID())->context();
      if (anExtRes)
        aSelection = anExtRes->shape();
    }
    // update arguments due to the selection value
    if (aSelection && !aSelection->isNull() && aSelection->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge( new GeomAPI_Edge(aSelection));
      std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_ID()));
      aStartAttr->setValue(sketch()->to2D(anEdge->firstPoint()));
      std::shared_ptr<GeomDataAPI_Point2D> anEndAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_ID()));
      anEndAttr->setValue(sketch()->to2D(anEdge->lastPoint()));
      updateLenghtValue();
    }
  }
  else if (theID == START_ID() || theID == END_ID()) {
    updateLenghtValue();
  }
}

void SketchPlugin_Line::updateLenghtValue()
{
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(END_ID()));
  if (aStartAttr->isInitialized() && anEndAttr->isInitialized()) {
    double aDistance = aStartAttr->pnt()->distance(anEndAttr->pnt());
    data()->real(LENGTH_ID())->setValue(aDistance);
  }
}
