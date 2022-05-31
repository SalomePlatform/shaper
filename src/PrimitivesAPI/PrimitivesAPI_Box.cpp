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

#include "PrimitivesAPI_Box.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
PrimitivesAPI_Box::PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
PrimitivesAPI_Box::PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Double & theDx,
                                     const ModelHighAPI_Double & theDy,
                                     const ModelHighAPI_Double & theDz)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setDimensions(theDx, theDy, theDz);
}

//==================================================================================================
PrimitivesAPI_Box::PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Selection& theFirstPoint,
                                     const ModelHighAPI_Selection& theSecondPoint)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setPoints(theFirstPoint, theSecondPoint);
}

//==================================================================================================
PrimitivesAPI_Box::PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Double& theOx,
                                     const ModelHighAPI_Double& theOy,
                                     const ModelHighAPI_Double& theOz,
                                     const ModelHighAPI_Double& theHalfX,
                                     const ModelHighAPI_Double& theHalfY,
                                     const ModelHighAPI_Double& theHalfZ)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
  {
    fillAttribute(PrimitivesPlugin_Box::CREATION_METHOD_BY_ONE_POINT_AND_DIMS(), creationMethod());
    fillAttribute(theOx, ox());
    fillAttribute(theOy, oy());
    fillAttribute(theOz, oz());
    fillAttribute(theHalfX, halfdx());
    fillAttribute(theHalfY, halfdy());
    fillAttribute(theHalfZ, halfdz());
    execute();
  }
}

//==================================================================================================
PrimitivesAPI_Box::~PrimitivesAPI_Box()
{

}

//==================================================================================================
void PrimitivesAPI_Box::setDimensions(const ModelHighAPI_Double& theDx,
                                      const ModelHighAPI_Double& theDy,
                                      const ModelHighAPI_Double& theDz)
{
  fillAttribute(PrimitivesPlugin_Box::CREATION_METHOD_BY_DIMENSIONS(), creationMethod());
  fillAttribute(theDx, dx());
  fillAttribute(theDy, dy());
  fillAttribute(theDz, dz());

  execute();
}

//==================================================================================================
void PrimitivesAPI_Box::setPoints(const ModelHighAPI_Selection& theFirstPoint,
                                  const ModelHighAPI_Selection& theSecondPoint)
{
  fillAttribute(PrimitivesPlugin_Box::CREATION_METHOD_BY_TWO_POINTS(), creationMethod());
  fillAttribute(theFirstPoint, firstPoint());
  fillAttribute(theSecondPoint, secondPoint());

  execute();
}

//==================================================================================================
void PrimitivesAPI_Box::setOrigin(const ModelHighAPI_Double& theOx,
                                  const ModelHighAPI_Double& theOy,
                                  const ModelHighAPI_Double& theOz)
{
  fillAttribute(theOx, ox());
  fillAttribute(theOy, oy());
  fillAttribute(theOz, oz());

  execute();
}

//==================================================================================================
void PrimitivesAPI_Box::setHalfLengths(const ModelHighAPI_Double& theHalfLengthX,
                                       const ModelHighAPI_Double& theHalfLengthY,
                                       const ModelHighAPI_Double& theHalfLengthZ)
{
  fillAttribute(theHalfLengthX, halfdx());
  fillAttribute(theHalfLengthY, halfdy());
  fillAttribute(theHalfLengthZ, halfdz());

  execute();
}

//==================================================================================================
void PrimitivesAPI_Box::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addBox(" << aDocName;

  std::string aCreationMethod = aBase->string(PrimitivesPlugin_Box::CREATION_METHOD())->value();

  if(aCreationMethod == PrimitivesPlugin_Box::CREATION_METHOD_BY_DIMENSIONS()) {
    AttributeDoublePtr anAttrDx = aBase->real(PrimitivesPlugin_Box::DX_ID());
    AttributeDoublePtr anAttrDy = aBase->real(PrimitivesPlugin_Box::DY_ID());
    AttributeDoublePtr anAttrDz = aBase->real(PrimitivesPlugin_Box::DZ_ID());
    theDumper << ", " << anAttrDx << ", " << anAttrDy << ", " << anAttrDz;
  } else if (aCreationMethod == PrimitivesPlugin_Box::CREATION_METHOD_BY_TWO_POINTS()) {
    AttributeSelectionPtr anAttrFirstPnt =
      aBase->selection(PrimitivesPlugin_Box::POINT_FIRST_ID());
    AttributeSelectionPtr anAttrSecondPnt =
      aBase->selection(PrimitivesPlugin_Box::POINT_SECOND_ID());
    theDumper << ", " << anAttrFirstPnt << ", " << anAttrSecondPnt;
  } else if (aCreationMethod == PrimitivesPlugin_Box::CREATION_METHOD_BY_ONE_POINT_AND_DIMS()) {
    AttributeDoublePtr anAttrOx = aBase->real(PrimitivesPlugin_Box::OX_ID());
    AttributeDoublePtr anAttrOy = aBase->real(PrimitivesPlugin_Box::OY_ID());
    AttributeDoublePtr anAttrOz = aBase->real(PrimitivesPlugin_Box::OZ_ID());
    AttributeDoublePtr anAttrHalfLengthX = aBase->real(PrimitivesPlugin_Box::HALF_DX_ID());
    AttributeDoublePtr anAttrHalfLengthY = aBase->real(PrimitivesPlugin_Box::HALF_DY_ID());
    AttributeDoublePtr anAttrHalfLengthZ = aBase->real(PrimitivesPlugin_Box::HALF_DZ_ID());
    theDumper << ", " << anAttrOx << ", " << anAttrOy << ", " << anAttrOz;
    theDumper << ", " << anAttrHalfLengthX << ", " << anAttrHalfLengthY;
    theDumper << ", " << anAttrHalfLengthZ;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
BoxPtr addBox(const std::shared_ptr<ModelAPI_Document>& thePart,
              const ModelHighAPI_Double& theDx,
              const ModelHighAPI_Double& theDy,
              const ModelHighAPI_Double& theDz)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Box::ID());
  return BoxPtr(new PrimitivesAPI_Box(aFeature, theDx, theDy, theDz));
}

//==================================================================================================
BoxPtr addBox(const std::shared_ptr<ModelAPI_Document>& thePart,
              const ModelHighAPI_Selection& theFirstPoint,
              const ModelHighAPI_Selection& theSecondPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Box::ID());
  return BoxPtr(new PrimitivesAPI_Box(aFeature, theFirstPoint, theSecondPoint));
}

//==================================================================================================
BoxPtr addBox(const std::shared_ptr<ModelAPI_Document>& thePart,
              const ModelHighAPI_Double& theOx,
              const ModelHighAPI_Double& theOy,
              const ModelHighAPI_Double& theOz,
              const ModelHighAPI_Double& theHalfLengthX,
              const ModelHighAPI_Double& theHalfLengthY,
              const ModelHighAPI_Double& theHalfLengthZ)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Box::ID());
  return BoxPtr(new PrimitivesAPI_Box(aFeature, theOx, theOy, theOz, theHalfLengthX,
                                      theHalfLengthY, theHalfLengthZ));
}
