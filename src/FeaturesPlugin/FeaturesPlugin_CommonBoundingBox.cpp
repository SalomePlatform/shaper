// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_CommonBoundingBox.h"

#include <ModelAPI_AttributeDoubleArray.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <GeomAPI_Vertex.h>

#include <GeomAlgoAPI_BoundingBox.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <Config_PropManager.h>

#include <iomanip>
#include <sstream>



//=================================================================================================
void FeaturesPlugin_CommonBoundingBox::createBoxByTwoPoints()
{
  AttributeDoubleArrayPtr aValues =
      std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(attributResultValues());

  SessionPtr aSession = ModelAPI_Session::get();

  DocumentPtr aDoc =  aSession->activeDocument();

  GeomVertexPtr vertexFirst =
          GeomAlgoAPI_PointBuilder::vertex(aValues->value(0),
                                           aValues->value(2),
                                           aValues->value(4));

  GeomVertexPtr  vertexSecond =
          GeomAlgoAPI_PointBuilder::vertex(aValues->value(1),
                                           aValues->value(3),
                                           aValues->value(5));


  std::shared_ptr<GeomAlgoAPI_Box> aBoxAlgo;


  aBoxAlgo = std::shared_ptr<GeomAlgoAPI_Box>(
                          new GeomAlgoAPI_Box(vertexFirst->point(),vertexSecond->point()));


  // These checks should be made to the GUI for the feature but
  // the corresponding validator does not exist yet.
  if (!aBoxAlgo->check()) {
    setError(aBoxAlgo->getError());
    return;
  }

  // Build the box
  aBoxAlgo->build();

  // Check if the creation of the box
  if (!aBoxAlgo->isDone()) {
    // The error is not displayed in a popup window. It must be in the message console.
    setError(aBoxAlgo->getError());
    return;
  }
  if (!aBoxAlgo->checkValid("Box builder with two points")) {
    // The error is not displayed in a popup window. It must be in the message console.
    setError(aBoxAlgo->getError());
    return;
  }

  int aResultIndex = 0;
  ResultBodyPtr aResultBox = document()->createBody(data(), aResultIndex);
  loadNamingDS(aBoxAlgo, aResultBox);
  setResult(aResultBox, aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_CommonBoundingBox::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Box> theBoxAlgo,
                                        std::shared_ptr<ModelAPI_ResultBody> theResultBox)
{
  // Load the result
  theResultBox->store(theBoxAlgo->shape());

  // Prepare the naming
  theBoxAlgo->prepareNamingFaces();

  // Insert to faces
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
    theBoxAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator it = listOfFaces.begin();
       it != listOfFaces.end();
       ++it) {
    theResultBox->generated((*it).second, (*it).first);
  }
}
