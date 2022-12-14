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

#include <FeaturesPlugin_LimitTolerance.h>

#include <GeomAlgoAPI_LimitTolerance.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Shape.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_ResultBody.h>



//=================================================================================================
FeaturesPlugin_LimitTolerance::FeaturesPlugin_LimitTolerance()
{
}

//=================================================================================================
void FeaturesPlugin_LimitTolerance::initAttributes()
{
  data()->addAttribute(OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TOLERANCE_ID(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void FeaturesPlugin_LimitTolerance::execute()
{
  // Get all feature arguments
  GeomShapePtr aShape = selection(FeaturesPlugin_LimitTolerance::OBJECT_ID())->value();
  double aTolerance = real(FeaturesPlugin_LimitTolerance::TOLERANCE_ID())->value();

  std::shared_ptr<GeomAlgoAPI_LimitTolerance> aLimitToleranceAlgo(new GeomAlgoAPI_LimitTolerance(aShape, aTolerance));

  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aLimitToleranceAlgo, getKind(), anError))
  {
    setError(anError);
    return;
  }
    
  // Store result.
  GeomShapePtr aResult = aLimitToleranceAlgo->shape();

  int anIndex = 0;
  ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
  aResultBody->storeModified(aShape, aResult);

  setResult(aResultBody, anIndex);
}
