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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "FeaturesPlugin_Recover.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>
#include <GeomAlgoAPI_Copy.h>

FeaturesPlugin_Recover::FeaturesPlugin_Recover()
{
}

void FeaturesPlugin_Recover::initAttributes()
{
  data()->addAttribute(BASE_FEATURE(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(RECOVERED_ENTITIES(), ModelAPI_AttributeRefList::typeId());

}

void FeaturesPlugin_Recover::execute()
{
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
    GeomAlgoAPI_Copy aCopyAlgo(aShape);
    // Check that algo is done.
    if(!aCopyAlgo.isDone()) {
      setError("Error: recover algorithm failed.");
      return;
    }
    // Check if shape is not null.
    if(!aCopyAlgo.shape().get() || aCopyAlgo.shape()->isNull()) {
      setError("Error: resulting shape is null.");
      return;
    }
    // Check that resulting shape is valid.
    if(!aCopyAlgo.isValid()) {
      setError("Error: resulting shape is not valid.");
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->store(aCopyAlgo.shape());//, aCopyAlgo.shape());
    std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = aCopyAlgo.mapOfSubShapes();
    // like in import: forget any history
    int aTag(1);
    std::string aNameMS = "Shape";
    aResultBody->loadFirstLevel(aCopyAlgo.shape(), aNameMS, aTag);

    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  removeResults(aResultIndex);
}
