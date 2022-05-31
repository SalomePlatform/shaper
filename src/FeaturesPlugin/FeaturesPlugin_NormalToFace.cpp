// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_NormalToFace.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Edge.h>
#include <Config_PropManager.h>
#include <GeomAlgoAPI_NormalToFace.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Dir.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <ModelAPI_ResultConstruction.h>

#include <iomanip>
#include <sstream>

//=================================================================================================
FeaturesPlugin_NormalToFace::FeaturesPlugin_NormalToFace()
{
}

//=================================================================================================
void FeaturesPlugin_NormalToFace::initAttributes()
{
  // attribute for object selected
  data()->addAttribute(FACE_SELECTED_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(VERTEX_SELECTED_ID(), ModelAPI_AttributeSelection::typeId());

  // attributes for result message and values
  data()->addAttribute(VERTEX_OPTION_ID(), ModelAPI_AttributeString::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VERTEX_SELECTED_ID());
}

//=================================================================================================
void FeaturesPlugin_NormalToFace::attributeChanged(const std::string& /*theID*/)
{
}

//=================================================================================================
void FeaturesPlugin_NormalToFace::execute()
{

  AttributeSelectionPtr aSelectionFace = selection(FACE_SELECTED_ID());
  AttributeSelectionPtr aSelectionPoint = selection(VERTEX_SELECTED_ID());

  GeomShapePtr aShape;
  GeomShapePtr aShapePoint;
  if (!string(VERTEX_OPTION_ID())->value().empty()) {
    if (aSelectionPoint && aSelectionPoint->isInitialized()) {
      aShapePoint = aSelectionPoint->value();
      if (!aShapePoint && aSelectionPoint->context())
        aShapePoint = aSelectionPoint->context()->shape();
      }
  }

  if (aSelectionFace && aSelectionFace->isInitialized()) {
    aShape = aSelectionFace->value();
    if (!aShape && aSelectionFace->context())
      aShape = aSelectionFace->context()->shape();
  }

  if (aShape) {
    std::string aError;
    std::shared_ptr<GeomAPI_Edge> theNormal(new GeomAPI_Edge);
    if (!GeomAlgoAPI_NormalToFace::normal(aShape,
                                          aShapePoint,
                                          theNormal,
                                          aError))
      setError("Error in bounding box calculation :" +  aError);

    GeomDirPtr theDir;
    std::shared_ptr<GeomAPI_Pnt> aPnt = theNormal->lastPoint();
    if (theNormal.get()) {
      if (theNormal->isLine()) {
        theDir = theNormal->line()->direction();
      }
    }
    aPnt->translate(theDir, 100);

    std::shared_ptr<GeomAPI_Edge> anEdge =
                        GeomAlgoAPI_EdgeBuilder::line(theNormal->firstPoint(),
                                                      aPnt);

    ResultConstructionPtr aConstr = document()->createConstruction(data());
    aConstr->setInfinite(true);
    aConstr->setShape(anEdge);
    setResult(aConstr);
  }
}


