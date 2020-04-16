// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "ModelAPI_Folder.h"

#include <ModelAPI_AttributeReference.h>

ModelAPI_Folder::ModelAPI_Folder()
{
}

ModelAPI_Folder::~ModelAPI_Folder()
{
}

void ModelAPI_Folder::init()
{
}

void ModelAPI_Folder::initAttributes()
{
  data()->addAttribute(FIRST_FEATURE_ID(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(LAST_FEATURE_ID(),  ModelAPI_AttributeReference::typeId());
}

void ModelAPI_Folder::execute()
{
}

std::shared_ptr<ModelAPI_Feature> ModelAPI_Folder::lastVisibleFeature()
{
  FeaturePtr aResult;
  AttributeReferencePtr aLastFeatAttr = data()->reference(LAST_FEATURE_ID());
  if (!aLastFeatAttr.get())
    return aResult;
  aResult = ModelAPI_Feature::feature(aLastFeatAttr->value());
  while(aResult.get() && !aResult->isInHistory()) { // searching for previous feature
    aResult = aResult->document()->nextFeature(aResult, true);
  }
  return aResult;
}
