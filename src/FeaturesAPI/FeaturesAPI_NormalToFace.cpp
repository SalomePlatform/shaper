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

#include "FeaturesAPI_NormalToFace.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//=================================================================================================
FeaturesAPI_NormalToFace::FeaturesAPI_NormalToFace(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//=================================================================================================
FeaturesAPI_NormalToFace::FeaturesAPI_NormalToFace(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const ModelHighAPI_Selection& theBaseFace,
                                    const ModelHighAPI_Selection& theOptionalPoint)
:ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theBaseFace, myfaceSelected);
    fillAttribute(theOptionalPoint, myvertexSelected);
    feature()->string(FeaturesPlugin_NormalToFace::VERTEX_OPTION_ID())->setValue("true");
    execute();
  }
}

//=================================================================================================
FeaturesAPI_NormalToFace::FeaturesAPI_NormalToFace(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const ModelHighAPI_Selection& theBaseFace)
:ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theBaseFace, myfaceSelected);
    feature()->string(FeaturesPlugin_NormalToFace::VERTEX_OPTION_ID())->setValue("");
    execute();
  }
}

//=================================================================================================
FeaturesAPI_NormalToFace::~FeaturesAPI_NormalToFace()
{
}

//=================================================================================================
void FeaturesAPI_NormalToFace::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrObject;
    anAttrObject = aBase->selection(FeaturesPlugin_NormalToFace::FACE_SELECTED_ID());

  theDumper << aBase << " = model.getNormal(" << aDocName << ", " << anAttrObject;

  if (!aBase->string(FeaturesPlugin_NormalToFace::VERTEX_OPTION_ID())->value().empty()) {

    AttributeSelectionPtr anAttrVertex =
            aBase->selection(FeaturesPlugin_NormalToFace::VERTEX_SELECTED_ID());

    theDumper << ", " << anAttrVertex;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
NormalPtr getNormal(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theBaseFace,
                    const ModelHighAPI_Selection& theOptionalPoint)
{

  FeaturePtr aFeature = thePart->addFeature(FeaturesPlugin_NormalToFace::ID());
  NormalPtr aNormalToface;
  aNormalToface.reset(new FeaturesAPI_NormalToFace(aFeature, theBaseFace, theOptionalPoint));

  return aNormalToface;
}

//=================================================================================================
NormalPtr getNormal(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theBaseFace)
{

  FeaturePtr aFeature = thePart->addFeature(FeaturesPlugin_NormalToFace::ID());
  NormalPtr aNormalToface;
  aNormalToface.reset(new FeaturesAPI_NormalToFace(aFeature, theBaseFace));

  return aNormalToface;
}

