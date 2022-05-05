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

#include "FeaturesPlugin_Recover.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>
#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_Tools.h>

FeaturesPlugin_Recover::FeaturesPlugin_Recover()
  : myClearListOnTypeChange(true)
{
}

void FeaturesPlugin_Recover::initAttributes()
{
  data()->addAttribute(BASE_FEATURE(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(RECOVERED_ENTITIES(), ModelAPI_AttributeRefList::typeId());

  data()->addAttribute(METHOD(), ModelAPI_AttributeString::typeId());
  if (!string(METHOD())->isInitialized()) {
    myClearListOnTypeChange = false;
    data()->blockSendAttributeUpdated(true, false);
    string(METHOD())->setValue(METHOD_DEFAULT());
    data()->blockSendAttributeUpdated(false, false);
    myClearListOnTypeChange = true;
  }
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), METHOD());
}

void FeaturesPlugin_Recover::execute()
{
  std::string anError;
  int aResultIndex = 0;
  AttributeRefListPtr aRecovered = reflist(RECOVERED_ENTITIES());
  for(int anIndex = aRecovered->size() - 1; anIndex >= 0; anIndex--) {
    ObjectPtr anObj = aRecovered->object(anIndex);
    if (!anObj.get())
      continue;
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (!aResult.get())
      continue;
    GeomShapePtr aShape = aResult->shape();
    if (!aShape.get())
      continue;

    // Copy shape.
    std::shared_ptr<GeomAlgoAPI_Copy> aCopyAlgo(new GeomAlgoAPI_Copy(aShape));
    // Check that algo is done.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCopyAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->store(aCopyAlgo->shape());//, aCopyAlgo.shape());

    aResultBody->loadModifiedShapes(aCopyAlgo, aShape, GeomAPI_Shape::VERTEX);
    aResultBody->loadModifiedShapes(aCopyAlgo, aShape, GeomAPI_Shape::EDGE);
    aResultBody->loadModifiedShapes(aCopyAlgo, aShape, GeomAPI_Shape::FACE);

    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  removeResults(aResultIndex);
}

void FeaturesPlugin_Recover::attributeChanged(const std::string& theID)
{
  if (theID == METHOD() && myClearListOnTypeChange)
    reflist(RECOVERED_ENTITIES())->clear();
}
