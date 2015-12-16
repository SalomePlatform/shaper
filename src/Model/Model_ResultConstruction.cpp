// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultConstruction.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultConstruction.h>

#include <ModelAPI_AttributeIntArray.h>
#include <Config_PropManager.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAlgoAPI_SketchBuilder.h>

void Model_ResultConstruction::initAttributes()
{
  // append the color attribute. It is empty, the attribute will be filled by a request
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::typeId());
}

void Model_ResultConstruction::colorConfigInfo(std::string& theSection, std::string& theName,
                                       std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_construction_color";
  theDefault = DEFAULT_COLOR();
}

void Model_ResultConstruction::setShape(std::shared_ptr<GeomAPI_Shape> theShape)
{
  if (myShape != theShape && (!theShape.get() || !theShape->isEqual(myShape))) {
    myShape = theShape;
    if (theShape.get()) {
      myFacesUpToDate = false;
      myFaces.clear();
    }
  }
}

std::shared_ptr<GeomAPI_Shape> Model_ResultConstruction::shape()
{
  return myShape;
}

Model_ResultConstruction::Model_ResultConstruction()
{
  myIsInHistory = true;
  myIsInfinite = false;
  myFacesUpToDate = false;
}

void Model_ResultConstruction::setIsInHistory(const bool isInHistory)
{
  myIsInHistory = isInHistory;
}

int Model_ResultConstruction::facesNum()
{
  if (!myFacesUpToDate) {
    std::shared_ptr<GeomAPI_PlanarEdges> aWirePtr = 
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(myShape);
    if (aWirePtr.get()) {
      std::list<std::shared_ptr<GeomAPI_Shape> > aFaces;
      GeomAlgoAPI_SketchBuilder::createFaces(aWirePtr->origin(), aWirePtr->dirX(),
        aWirePtr->norm(), aWirePtr, aFaces);
      std::list<std::shared_ptr<GeomAPI_Shape> >::iterator aFIter = aFaces.begin();
      for(; aFIter != aFaces.end(); aFIter++) {
        std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(*aFIter));
        if (aFace.get() && !aFace->isNull())
          myFaces.push_back(aFace);
      }
    }
    myFacesUpToDate = true;
  }
  return myFaces.size();
}

std::shared_ptr<GeomAPI_Face> Model_ResultConstruction::face(const int theIndex)
{
  return myFaces[theIndex];
}

bool Model_ResultConstruction::isInfinite()
{
  return myIsInfinite;
}

void Model_ResultConstruction::setInfinite(const bool theInfinite)
{
  myIsInfinite = theInfinite;
}

void Model_ResultConstruction::setIsConcealed(const bool theValue)
{
  // do nothing: the construction element is never consealed
}
