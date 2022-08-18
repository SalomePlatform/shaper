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

#include "FeaturesAPI_PointCloudOnFace.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//=================================================================================================
FeaturesAPI_PointCloudOnFace::FeaturesAPI_PointCloudOnFace(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//=================================================================================================
FeaturesAPI_PointCloudOnFace::FeaturesAPI_PointCloudOnFace(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const ModelHighAPI_Selection& theFace,
                                    const ModelHighAPI_Integer& theNumber)
:ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theFace, myfaceSelected);
    setNumberOfPoints(theNumber);
  }
}


//=================================================================================================
FeaturesAPI_PointCloudOnFace::~FeaturesAPI_PointCloudOnFace()
{
}

//==================================================================================================
void FeaturesAPI_PointCloudOnFace::setNumberOfPoints(const ModelHighAPI_Integer& theNumber)
{
  fillAttribute(theNumber, nbPoints());

  execute();
}

//=================================================================================================
void FeaturesAPI_PointCloudOnFace::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrObject =
    aBase->selection(FeaturesPlugin_PointCloudOnFace::FACE_SELECTED_ID());
  int aNbPnts = aBase->integer(FeaturesPlugin_PointCloudOnFace::NUMBER_ID())->value();

  theDumper << aBase << " = model.makeVertexInsideFace(" << aDocName
            << ", " << anAttrObject << ", " << aNbPnts << ")" << std::endl;
}

//==================================================================================================
PointCloudPtr makeVertexInsideFace(const std::shared_ptr<ModelAPI_Document>& thePart,
                                   const ModelHighAPI_Selection& theFace,
                                   const ModelHighAPI_Integer& theNumber)
{
  FeaturePtr aFeature = thePart->addFeature(FeaturesPlugin_PointCloudOnFace::ID());
  PointCloudPtr aPointCloud;
  aPointCloud.reset(new FeaturesAPI_PointCloudOnFace(aFeature, theFace, theNumber));

  return aPointCloud;
}
