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

#include "ModelAPI_ResultBody.h"

#include <ModelAPI_BodyBuilder.h>
#include <Events_Loop.h>
#include <ModelAPI_Events.h>

ModelAPI_ResultBody::ModelAPI_ResultBody()
  : myBuilder(0)
{
  myConnect = ConnectionNotComputed;
}

ModelAPI_ResultBody::~ModelAPI_ResultBody()
{
}

std::string ModelAPI_ResultBody::groupName()
{
  return group();
}

void ModelAPI_ResultBody::store(const GeomShapePtr& theShape,
                                const bool theIsStoreSameShapes)
{
  myBuilder->store(theShape, theIsStoreSameShapes);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);

  updateSubs(theShape);
}

void ModelAPI_ResultBody::storeGenerated(const GeomShapePtr& theFromShape,
                                         const GeomShapePtr& theToShape)
{
  myBuilder->storeGenerated(theFromShape, theToShape);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);

  updateSubs(theToShape);
}

void ModelAPI_ResultBody::storeModified(const GeomShapePtr& theOldShape,
                                        const GeomShapePtr& theNewShape,
                                        const bool theIsCleanStored)
{
  myBuilder->storeModified(theOldShape, theNewShape, theIsCleanStored);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);

  updateSubs(theNewShape);
}

void ModelAPI_ResultBody::storeWithoutNaming(const GeomShapePtr& theShape)
{
  myBuilder->storeWithoutNaming(theShape);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);
}

GeomShapePtr ModelAPI_ResultBody::shape()
{
  return myBuilder->shape();
}

void ModelAPI_ResultBody::generated(const GeomShapePtr& theNewShape,
                                    const std::string& theName)
{
  myBuilder->generated(theNewShape, theName);
}

void ModelAPI_ResultBody::generated(const GeomShapePtr& theOldShape,
                                    const GeomShapePtr& theNewShape,
                                    const std::string& theName)
{
  myBuilder->generated(theOldShape, theNewShape, theName);
}

void ModelAPI_ResultBody::modified(const GeomShapePtr& theOldShape,
                                   const GeomShapePtr& theNewShape,
                                   const std::string& theName)
{
  myBuilder->modified(theOldShape, theNewShape, theName);
}


void ModelAPI_ResultBody::deleted(const GeomShapePtr& theOldShape)
{
  myBuilder->deleted(theOldShape);
}

void ModelAPI_ResultBody::loadDeletedShapes(const GeomMakeShapePtr& theAlgo,
                                            const GeomShapePtr& theOldShape,
                                            const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                            const GeomShapePtr& theShapesToExclude)
{
  myBuilder->loadDeletedShapes(theAlgo, theOldShape, theShapeTypeToExplore, theShapesToExclude);
}

void ModelAPI_ResultBody::loadModifiedShapes(const GeomMakeShapePtr& theAlgo,
                                             const GeomShapePtr& theOldShape,
                                             const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                             const std::string& theName)
{
  myBuilder->loadModifiedShapes(theAlgo, theOldShape, theShapeTypeToExplore, theName);
}

void ModelAPI_ResultBody::loadGeneratedShapes(const GeomMakeShapePtr& theAlgo,
                                              const GeomShapePtr& theOldShape,
                                              const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                              const std::string& theName)
{
  myBuilder->loadGeneratedShapes(theAlgo, theOldShape, theShapeTypeToExplore, theName);
}

void ModelAPI_ResultBody::loadFirstLevel(GeomShapePtr theShape,
                                         const std::string& theName,
                                         int& theTag)
{
  myBuilder->loadFirstLevel(theShape, theName, theTag);
}

void ModelAPI_ResultBody::loadDisconnectedEdges(GeomShapePtr theShape,
                                                const std::string& theName,
                                                int& theTag)
{
  myBuilder->loadDisconnectedEdges(theShape, theName, theTag);
}

void ModelAPI_ResultBody::loadDisconnectedVertexes(GeomShapePtr theShape,
                                                   const std::string& theName,
                                                    int& theTag)
{
  myBuilder->loadDisconnectedVertexes(theShape, theName, theTag);
}

bool ModelAPI_ResultBody::isConnectedTopology()
{
  if (myConnect == ConnectionNotComputed) {
    myConnect = shape()->isConnectedTopology() ? IsConnected : IsNotConnected;
  }
  return myConnect == IsConnected;
}

void ModelAPI_ResultBody::setDisplayed(const bool theDisplay)
{
  ModelAPI_Result::setDisplayed(theDisplay);
  for (int i = 0; i < numberOfSubs(); i++)
    subResult(i)->setDisplayed(theDisplay);
}
