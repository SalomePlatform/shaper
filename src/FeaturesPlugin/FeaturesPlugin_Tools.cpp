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
