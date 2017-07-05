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

#include <GDMLPlugin_Ellipsoid.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>

//=================================================================================================
GDMLPlugin_Ellipsoid::GDMLPlugin_Ellipsoid() // Nothing to do during instantiation
{
}

//=================================================================================================
void GDMLPlugin_Ellipsoid::initAttributes()
{
  data()->addAttribute(GDMLPlugin_Ellipsoid::AX_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_Ellipsoid::BY_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_Ellipsoid::CZ_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_Ellipsoid::USE_ZCUT1_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(GDMLPlugin_Ellipsoid::ZCUT1_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(GDMLPlugin_Ellipsoid::USE_ZCUT2_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(GDMLPlugin_Ellipsoid::ZCUT2_ID(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void GDMLPlugin_Ellipsoid::execute()
{
  std::shared_ptr<GeomAlgoAPI_Ellipsoid> anEllipsoidAlgo;
  
  double aAx = real(AX_ID())->value();
  double aBy = real(BY_ID())->value();
  double aCz = real(CZ_ID())->value();
  
  std::string useZCut1 = string(USE_ZCUT1_ID())->value();
  std::string useZCut2 = string(USE_ZCUT2_ID())->value();
  
  double aZCut1 = 0.;
  if (useZCut1.empty()) {
    aZCut1 = aCz /2.;
  } else {
    aZCut1 = real(ZCUT1_ID())->value();
  }
  double aZCut2 = 0.;
  if (useZCut2.empty()) {
    aZCut2 = aCz /2.;
  } else {
    aZCut2 = real(ZCUT2_ID())->value();
  }
  
  anEllipsoidAlgo = std::shared_ptr<GeomAlgoAPI_Ellipsoid>(
    new GeomAlgoAPI_Ellipsoid(aAx, aBy, aCz, aZCut1, aZCut2));
  
  // Check with that the arguments for anEllipsoidAlgo are correct
  if (!anEllipsoidAlgo->check()){
    setError(anEllipsoidAlgo->getError(), false);   
    return;
  }

  anEllipsoidAlgo->build();

  // Check if the creation of the ellipsoid is correct
  if (!anEllipsoidAlgo->isDone()) {
    setError(anEllipsoidAlgo->getError(), false);
    return;
  }
  
  // Check if the created ellipsoid is valid
  if (!anEllipsoidAlgo->checkValid("Ellipsoid builder")) {
    setError(anEllipsoidAlgo->getError(), false);
    return;
  }

  int aResultIndex = 0; 
  ResultBodyPtr aResultEllipsoid = document()->createBody(data(), aResultIndex); 
  loadNamingDS(anEllipsoidAlgo, aResultEllipsoid);
  setResult(aResultEllipsoid, aResultIndex);

}

//=================================================================================================
void GDMLPlugin_Ellipsoid::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Ellipsoid> theEllipsoidAlgo,
                                        std::shared_ptr<ModelAPI_ResultBody> theResultEllipsoid)
{
  // Load the result
  theResultEllipsoid->store(theEllipsoidAlgo->shape());
  
  // Prepare the naming
  theEllipsoidAlgo->prepareNamingFaces();
  
  // Insert to faces
  int num = 1;
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
    theEllipsoidAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator
    it=listOfFaces.begin(); it!=listOfFaces.end(); ++it) {
    std::shared_ptr<GeomAPI_Shape> aFace = (*it).second;
    theResultEllipsoid->generated(aFace, (*it).first, num++);
  }
}

