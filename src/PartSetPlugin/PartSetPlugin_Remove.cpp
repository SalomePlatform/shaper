// Copyright (C) 2014-2023  CEA, EDF
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

#include "PartSetPlugin_Remove.h"
#include "PartSetPlugin_Part.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Events.h>

#include <Events_Loop.h>

void PartSetPlugin_Remove::execute()
{
  std::shared_ptr<ModelAPI_Session> aPManager = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aRoot = aPManager->moduleDocument();
  DocumentPtr aThisDoc = document();
  ResultPtr aPart = ModelAPI_Tools::findPartResult(aRoot, aThisDoc);
  if (aPart.get()) {
    FeaturePtr aFeature = aRoot->feature(aPart);
    if (aFeature) {
      // do remove, but don't do real close (features are erased without persistence changes
      // document remove may be undoed)
      // aPart->data()->document(ModelAPI_ResultPart::DOC_REF())->value()->close();
      std::set<std::shared_ptr<ModelAPI_Feature> > aRefFeatures;
      aRoot->refsToFeature(aFeature, aRefFeatures);
      if (aRefFeatures.empty()) {
        // before removing the part make the partSet active document
        if (aThisDoc == aPManager->activeDocument())
          aPManager->setActiveDocument(aRoot);
        aRoot->removeFeature(aFeature);
        // the redisplay signal should be flushed in order to erase the feature presentation
        // in the viewer after removeFeature from the document
        Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
      }
    }
  }
}
