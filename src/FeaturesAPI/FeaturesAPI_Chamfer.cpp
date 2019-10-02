// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_Chamfer.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

FeaturesAPI_Chamfer::FeaturesAPI_Chamfer(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

FeaturesAPI_Chamfer::FeaturesAPI_Chamfer(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                         const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                         const bool performDistances,
                                         const ModelHighAPI_Double& theVal1,
                                         const ModelHighAPI_Double& theVal2)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    if (performDistances) {
      fillAttribute(FeaturesPlugin_Chamfer::CREATION_METHOD_DISTANCE_DISTANCE(), mycreationMethod);
      fillAttribute(theVal1, myd1);
      fillAttribute(theVal2, myd2);
    } else {
      fillAttribute(FeaturesPlugin_Chamfer::CREATION_METHOD_DISTANCE_ANGLE(), mycreationMethod);
      fillAttribute(theVal1, myd);
      fillAttribute(theVal2, myangle);
    }

    execIfBaseNotEmpty();
  }
}

FeaturesAPI_Chamfer::~FeaturesAPI_Chamfer()
{
}

//==================================================================================================
void FeaturesAPI_Chamfer::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  mybaseObjects->clear();
  fillAttribute(theBaseObjects, mybaseObjects);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Chamfer::setDistances(const ModelHighAPI_Double& theD1,
                                       const ModelHighAPI_Double& theD2)
{
  fillAttribute(FeaturesPlugin_Chamfer::CREATION_METHOD_DISTANCE_DISTANCE(), mycreationMethod);
  fillAttribute(theD1, myd1);
  fillAttribute(theD2, myd2);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Chamfer::setDistAngle(const ModelHighAPI_Double& theDistance,
                                       const ModelHighAPI_Double& theAngle)
{
  fillAttribute(FeaturesPlugin_Chamfer::CREATION_METHOD_DISTANCE_ANGLE(), mycreationMethod);
  fillAttribute(theDistance, myd);
  fillAttribute(theAngle, myangle);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Chamfer::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Chamfer::OBJECT_LIST_ID());

  theDumper << aBase << " = model.addChamfer(" << aDocName << ", " << anAttrObjects;

  std::string aCreationMethod = aBase->string(FeaturesPlugin_Chamfer::CREATION_METHOD())->value();

  if(aCreationMethod == FeaturesPlugin_Chamfer::CREATION_METHOD_DISTANCE_DISTANCE()) {
    AttributeDoublePtr anAttrD1 = aBase->real(FeaturesPlugin_Chamfer::D1_ID());
    AttributeDoublePtr anAttrD2 = aBase->real(FeaturesPlugin_Chamfer::D2_ID());
    theDumper << ", True, " << anAttrD1 << ", " << anAttrD2;
  } else if(aCreationMethod == FeaturesPlugin_Chamfer::CREATION_METHOD_DISTANCE_ANGLE()) {
    AttributeDoublePtr anAttrD = aBase->real(FeaturesPlugin_Chamfer::D_ID());
    AttributeDoublePtr anAttrAngle = aBase->real(FeaturesPlugin_Chamfer::ANGLE_ID());
    theDumper << ", False, " << anAttrD << ", " << anAttrAngle;
  }

  theDumper << ")" << std::endl;
}

void FeaturesAPI_Chamfer::execIfBaseNotEmpty()
{
  if (mybaseObjects->size() > 0)
    execute();
}


//==================================================================================================

ChamferPtr addChamfer(const std::shared_ptr<ModelAPI_Document>& thePart,
                      const std::list<ModelHighAPI_Selection>& theBaseObjects,
                      const bool performDistances,
                      const ModelHighAPI_Double& theVal1,
                      const ModelHighAPI_Double& theVal2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Chamfer::ID());
  return ChamferPtr(new FeaturesAPI_Chamfer(aFeature, theBaseObjects, performDistances,
                                            theVal1, theVal2));
}
