// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Tools.cpp
// Created:     17 November 2016
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_ResultBody.h>

#include <GeomAPI_ShapeIterator.h>

void FeaturesPlugin_Tools::storeModifiedShapes(GeomAlgoAPI_MakeShape& theAlgo,
                                               std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                               std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                               int& theTag,
                                               const std::string theName,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  switch(theBaseShape->shapeType()) {
    case GeomAPI_Shape::COMPOUND: {
      for(GeomAPI_ShapeIterator anIt(theBaseShape); anIt.more(); anIt.next())
      {
        storeModifiedShapes(theAlgo, theResultBody, theBaseShape, theTag, theName, theSubShapes);
      }
      break;
    }
    case GeomAPI_Shape::COMPSOLID:
    case GeomAPI_Shape::SOLID:
    case GeomAPI_Shape::SHELL: {
      theResultBody->loadAndOrientModifiedShapes(&theAlgo,
                                theBaseShape, GeomAPI_Shape::FACE,
                                theTag, theName + "_Face", theSubShapes);
      if (theBaseShape->shapeType() == GeomAPI_Shape::COMPSOLID
          || theBaseShape->shapeType() == GeomAPI_Shape::SOLID) {
        break;
      }
      ++theTag;
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::WIRE: {
      theResultBody->loadAndOrientModifiedShapes(&theAlgo,
                                theBaseShape, GeomAPI_Shape::EDGE,
                                theTag, theName + "_Edge", theSubShapes);
      ++theTag;
    }
    case GeomAPI_Shape::EDGE: {
      theResultBody->loadAndOrientModifiedShapes(&theAlgo,
                              theBaseShape, GeomAPI_Shape::VERTEX,
                              theTag, theName + "_Vertex", theSubShapes);
      ++theTag;
    }
  }
}
