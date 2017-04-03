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
                                               const int theFaceTag,
                                               const int theEdgeTag,
                                               const int theVertexTag,
                                               const std::string theName,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  switch(theBaseShape->shapeType()) {
    case GeomAPI_Shape::COMPOUND: {
      for(GeomAPI_ShapeIterator anIt(theBaseShape); anIt.more(); anIt.next())
      {
        storeModifiedShapes(theAlgo,
                            theResultBody,
                            anIt.current(),
                            theFaceTag,
                            theEdgeTag,
                            theVertexTag,
                            theName,
                            theSubShapes);
      }
      break;
    }
    case GeomAPI_Shape::COMPSOLID:
    case GeomAPI_Shape::SOLID:
    case GeomAPI_Shape::SHELL: {
      theResultBody->loadAndOrientModifiedShapes(&theAlgo,
                                theBaseShape, GeomAPI_Shape::FACE,
                                theFaceTag, theName + "_Face", theSubShapes, false, true);
      if (theBaseShape->shapeType() == GeomAPI_Shape::COMPSOLID
          || theBaseShape->shapeType() == GeomAPI_Shape::SOLID) {
        break;
      }
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::WIRE: {
      theResultBody->loadAndOrientModifiedShapes(&theAlgo,
                                theBaseShape, GeomAPI_Shape::EDGE,
                                theEdgeTag, theName + "_Edge", theSubShapes, false, true);
    }
    case GeomAPI_Shape::EDGE: {
      theResultBody->loadAndOrientModifiedShapes(&theAlgo,
                              theBaseShape, GeomAPI_Shape::VERTEX,
                              theVertexTag, theName + "_Vertex", theSubShapes, false, true);
    }
  }
}
