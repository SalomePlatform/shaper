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

#include "PartSetPlugin_Duplicate.h"
#include "PartSetPlugin_Part.h"
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>

void PartSetPlugin_Duplicate::execute()
{
  std::shared_ptr<ModelAPI_Session> aPManager = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aRoot = aPManager->moduleDocument();
  std::shared_ptr<ModelAPI_ResultPart> aSource;  // searching for source document attribute
  for (int a = aRoot->size(getGroup()) - 1; a >= 0; a--) {
    aSource = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aRoot->object(getGroup(), a));
    if (aSource && aSource->data()
        && aSource->data()->document(ModelAPI_ResultPart::DOC_REF())->value()
            == aPManager->activeDocument())
      break;
    aSource.reset();
  }
  if (aSource) {
    // create a new Part feature
    FeaturePtr aNewPart = aRoot->addFeature(PartSetPlugin_Part::ID(), false);
    aNewPart->execute(); // to make result and generate a unique name

    std::shared_ptr<ModelAPI_Document> aCopy = aPManager->copy(
      aSource->data()->document(ModelAPI_ResultPart::DOC_REF())->value(),
      std::dynamic_pointer_cast<ModelAPI_ResultPart>(aNewPart->firstResult())->partDoc()->id());
  }
}
