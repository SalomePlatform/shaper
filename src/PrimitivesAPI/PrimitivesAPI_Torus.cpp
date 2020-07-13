// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

// File:        PrimitivesAPI_Torus.cpp
// Created:     20 Mar 2017
// Author:      Clarisse Genrault

#include "PrimitivesAPI_Torus.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
PrimitivesAPI_Torus::PrimitivesAPI_Torus(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
PrimitivesAPI_Torus::PrimitivesAPI_Torus(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBasePoint,
                                       const ModelHighAPI_Selection& theAxis,
                                       const ModelHighAPI_Double& theRadius,
                                       const ModelHighAPI_Double& theRingRadius)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theBasePoint, basePoint());
    fillAttribute(theAxis, axis());
    setRadius(theRadius, theRingRadius);
  }
}

//==================================================================================================
PrimitivesAPI_Torus::~PrimitivesAPI_Torus()
{
}

//==================================================================================================
void PrimitivesAPI_Torus::setRadius(const ModelHighAPI_Double& theRadius,
                                    const ModelHighAPI_Double& theRingRadius)
{
  fillAttribute(theRadius, radius());
  fillAttribute(theRingRadius, ringRadius());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Torus::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addTorus(" << aDocName;

  AttributeSelectionPtr anAttrBasePoint =
      aBase->selection(PrimitivesPlugin_Torus::BASE_POINT_ID());
  AttributeSelectionPtr anAttrAxis =
      aBase->selection(PrimitivesPlugin_Torus::AXIS_ID());
  theDumper << ", " << anAttrBasePoint << ", " << anAttrAxis;

  AttributeDoublePtr anAttrRadius = aBase->real(PrimitivesPlugin_Torus::RADIUS_ID());
  AttributeDoublePtr anAttrRingRadius = aBase->real(PrimitivesPlugin_Torus::RING_RADIUS_ID());
  theDumper << ", " << anAttrRadius << ", " << anAttrRingRadius;

  theDumper << ")" << std::endl;
}

//==================================================================================================
TorusPtr addTorus(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& theBasePoint,
                  const ModelHighAPI_Selection& theAxis,
                  const ModelHighAPI_Double& theRadius,
                  const ModelHighAPI_Double& theRingRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Torus::ID());
  return TorusPtr(new PrimitivesAPI_Torus(aFeature, theBasePoint, theAxis,
                                          theRadius, theRingRadius));
}

//==================================================================================================
TorusPtr addTorus(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Double& theRadius,
                  const ModelHighAPI_Double& theRingRadius)
{
  ModelHighAPI_Selection aBasePoint("VERTEX", L"PartSet/Origin");
  ModelHighAPI_Selection anAxis("EDGE", L"PartSet/OZ");
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Torus::ID());
  return TorusPtr(new PrimitivesAPI_Torus(aFeature, aBasePoint, anAxis, theRadius, theRingRadius));
}
