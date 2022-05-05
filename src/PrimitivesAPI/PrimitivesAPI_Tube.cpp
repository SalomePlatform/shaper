// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

#include "PrimitivesAPI_Tube.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
PrimitivesAPI_Tube::PrimitivesAPI_Tube(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
PrimitivesAPI_Tube::PrimitivesAPI_Tube(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Double& theRMin,
                                       const ModelHighAPI_Double& theRMax,
                                       const ModelHighAPI_Double& theHeight)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theRMin, rmin());
    fillAttribute(theRMax, rmax());
    setHeight(theHeight);
  }
}

//==================================================================================================
PrimitivesAPI_Tube::~PrimitivesAPI_Tube()
{
}

//==================================================================================================
void PrimitivesAPI_Tube::setRadius(const ModelHighAPI_Double& theRMin,
                                   const ModelHighAPI_Double& theRMax)
{
  fillAttribute(theRMin, rmin());
  fillAttribute(theRMax, rmax());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Tube::setHeight(const ModelHighAPI_Double& theHeight)
{
  fillAttribute(theHeight, height());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Tube::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addTube(" << aDocName;

  AttributeDoublePtr anAttrRMin = aBase->real(PrimitivesPlugin_Tube::RMIN_ID());
  AttributeDoublePtr anAttrRMax = aBase->real(PrimitivesPlugin_Tube::RMAX_ID());
  AttributeDoublePtr anAttrHeight = aBase->real(PrimitivesPlugin_Tube::HEIGHT_ID());
  theDumper << ", " << anAttrRMin << ", " << anAttrRMax << ", " << anAttrHeight;

  theDumper << ")" << std::endl;
}

//==================================================================================================
TubePtr addTube(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Double& theRMin,
                const ModelHighAPI_Double& theRMax,
                const ModelHighAPI_Double& theHeight)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Tube::ID());
  return TubePtr(new PrimitivesAPI_Tube(aFeature, theRMin, theRMax, theHeight));
}
