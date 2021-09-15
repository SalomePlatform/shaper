// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_InspectNormalToFace.h"

#include <FeaturesPlugin_NormalToFace.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Vertex.h>

#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_NormalToFace.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <Config_PropManager.h>

#include <iomanip>
#include <sstream>

//=================================================================================================
FeaturesPlugin_InspectNormalToFace::FeaturesPlugin_InspectNormalToFace() {}

//=================================================================================================
void FeaturesPlugin_InspectNormalToFace::initAttributes()
{
  // attribute for object selected
  data()->addAttribute(FACE_SELECTED_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(VERTEX_SELECTED_ID(), ModelAPI_AttributeSelection::typeId());
  // attributes for result message and values
  data()->addAttribute(CREATENORMAL_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(VERTEX_OPTION_ID(), ModelAPI_AttributeString::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VERTEX_SELECTED_ID());

}

//=================================================================================================
void FeaturesPlugin_InspectNormalToFace::execute()
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
                      GeomAlgoAPI_EdgeBuilder::line(theNormal->firstPoint(), aPnt);

    ResultConstructionPtr aConstr = document()->createConstruction(data());
    aConstr->setInfinite(true);
    aConstr->setShape(anEdge);
    setResult(aConstr);
  }

  if (boolean(CREATENORMAL_ID())->value()) {
    if (!myCreateFeature.get())
      createNormal();
    updateNormal();
  } else {
    if (myCreateFeature.get()) {
      myCreateFeature->eraseResults();
      SessionPtr aSession = ModelAPI_Session::get();
      DocumentPtr aDoc =  aSession->activeDocument();
      aDoc->removeFeature(myCreateFeature);
      myCreateFeature.reset();
    }
  }
}

//=================================================================================================
void FeaturesPlugin_InspectNormalToFace::attributeChanged(const std::string& theID)
{
   if (theID == FACE_SELECTED_ID()) {
    if (myCreateFeature.get())
      updateNormal();
  }
}

//=================================================================================================
void FeaturesPlugin_InspectNormalToFace::createNormal()
{
  SessionPtr aSession = ModelAPI_Session::get();

  DocumentPtr aDoc =  aSession->activeDocument();

  if (aDoc.get()) {
    myCreateFeature = aDoc->addFeature(FeaturesPlugin_NormalToFace::ID());
  }
}

//=================================================================================================
void FeaturesPlugin_InspectNormalToFace::updateNormal()
{
  myCreateFeature->selection(FeaturesPlugin_NormalToFace::FACE_SELECTED_ID())
                        ->setValue(selection(FACE_SELECTED_ID())->context() ,
                                    selection(FACE_SELECTED_ID())->value());

  myCreateFeature->string(FeaturesPlugin_NormalToFace::VERTEX_OPTION_ID())
                        ->setValue(string(VERTEX_OPTION_ID())->value());

  if (!string(VERTEX_OPTION_ID())->value().empty()) {
    myCreateFeature->selection(FeaturesPlugin_NormalToFace::VERTEX_SELECTED_ID())
                        ->setValue(selection(VERTEX_SELECTED_ID())->context() ,
                                    selection(VERTEX_SELECTED_ID())->value());
  }

  myCreateFeature->execute();
}
