// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultBody.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_ResultBody.h"
#include <ModelAPI_BodyBuilder.h>

ModelAPI_ResultBody::ModelAPI_ResultBody()
: myBuilder(0)
{
}

ModelAPI_ResultBody::~ModelAPI_ResultBody()
{
  if (myBuilder)
  delete myBuilder;
}

std::string ModelAPI_ResultBody::groupName()
{
  return group();
}

void ModelAPI_ResultBody::store(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  myBuilder->store(theShape);
}

void ModelAPI_ResultBody::storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
	                          const std::shared_ptr<GeomAPI_Shape>& theToShape)
{
  myBuilder->storeGenerated(theFromShape, theToShape);
}

void ModelAPI_ResultBody::storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
	                          const std::shared_ptr<GeomAPI_Shape>& theNewShape,
                            const int theDecomposeSolidsTag)
{
  myBuilder->storeModified(theOldShape, theNewShape, theDecomposeSolidsTag);
}

void ModelAPI_ResultBody::storeWithoutNaming(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  myBuilder->storeWithoutNaming(theShape);
}

std::shared_ptr<GeomAPI_Shape> ModelAPI_ResultBody::shape()
{
  return myBuilder->shape();
}

void ModelAPI_ResultBody::generated(const std::shared_ptr<GeomAPI_Shape>& theNewShape,
    const std::string& theName, const int theTag)
{
  myBuilder->generated(theNewShape, theName, theTag);
}

void ModelAPI_ResultBody::generated(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, 
    const int theTag)
{
  myBuilder->generated(theOldShape, theNewShape, theName, theTag);
}

void ModelAPI_ResultBody::modified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName,
    const int theTag)
{
  myBuilder->modified(theOldShape, theNewShape, theName, theTag);
}


void ModelAPI_ResultBody::deleted(
    const std::shared_ptr<GeomAPI_Shape>& theOldShape, const int theTag)
{
  myBuilder->deleted(theOldShape, theTag);
}
  
void ModelAPI_ResultBody::loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
                                  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                  const int  theKindOfShape,
                                  const int  theTag)
{
  myBuilder->loadDeletedShapes(theMS, theShapeIn, theKindOfShape, theTag);
}

void ModelAPI_ResultBody::loadAndOrientModifiedShapes (GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape, const int  theTag,
    const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  myBuilder->loadAndOrientModifiedShapes(
    theMS, theShapeIn, theKindOfShape, theTag, theName, theSubShapes);
}

void ModelAPI_ResultBody::loadAndOrientGeneratedShapes (GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape,
    const int  theTag, const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  myBuilder->loadAndOrientGeneratedShapes(
    theMS, theShapeIn, theKindOfShape, theTag, theName, theSubShapes);
}

void ModelAPI_ResultBody::loadFirstLevel(std::shared_ptr<GeomAPI_Shape> theShape, 
    const std::string& theName, int&  theTag)
{
  myBuilder->loadFirstLevel(theShape, theName, theTag);
}

void ModelAPI_ResultBody::loadDisconnectedEdges(std::shared_ptr<GeomAPI_Shape> theShape,
    const std::string& theName, int&  theTag)
{
  myBuilder->loadDisconnectedEdges(theShape, theName, theTag);
}

void ModelAPI_ResultBody::loadDisconnectedVertexes(std::shared_ptr<GeomAPI_Shape> theShape,
    const std::string& theName,int&  theTag)
{
  myBuilder->loadDisconnectedVertexes(theShape, theName, theTag);
}
