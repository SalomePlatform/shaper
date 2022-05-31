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

#include <PrimitivesPlugin_Tube.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultBody.h>

//=================================================================================================
PrimitivesPlugin_Tube::PrimitivesPlugin_Tube() // Nothing to do during instantiation
{
}

//=================================================================================================
void PrimitivesPlugin_Tube::initAttributes()
{
  data()->addAttribute(PrimitivesPlugin_Tube::RMIN_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Tube::RMAX_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Tube::HEIGHT_ID(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void PrimitivesPlugin_Tube::execute()
{
  double aRMin = real(PrimitivesPlugin_Tube::RMIN_ID())->value();
  double aRMax = real(PrimitivesPlugin_Tube::RMAX_ID())->value();
  double aZ = real(PrimitivesPlugin_Tube::HEIGHT_ID())->value();

  std::shared_ptr<GeomAlgoAPI_Tube> aTubeAlgo(new GeomAlgoAPI_Tube(aRMin,aRMax,aZ));

  // These checks should be made to the GUI for the feature but
  // the corresponding validator does not exist yet.
  if (!aTubeAlgo->check()) {
    setError(aTubeAlgo->getError());
    return;
  }

  // Build the tube
  aTubeAlgo->build();

  int aResultIndex = 0;
  ResultBodyPtr aResultTube = document()->createBody(data(), aResultIndex);
  loadNamingDS(aTubeAlgo, aResultTube);
  setResult(aResultTube, aResultIndex);
}

//=================================================================================================
void PrimitivesPlugin_Tube::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Tube> theTubeAlgo,
                                         std::shared_ptr<ModelAPI_ResultBody> theResultTube)
{
  // Load the result
  theResultTube->store(theTubeAlgo->shape());

  // Prepare the naming
  theTubeAlgo->prepareNamingFaces();

  // Insert to faces
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
    theTubeAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator it = listOfFaces.begin();
       it != listOfFaces.end();
       ++it)
  {
    theResultTube->generated((*it).second, (*it).first);
  }
}
