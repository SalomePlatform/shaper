// Copyright (C) 2014-201x CEA/DEN, EDF R&D -->

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
  ModelHighAPI_Selection aBasePoint("VERTEX", "PartSet/Origin");
  ModelHighAPI_Selection anAxis("EDGE", "PartSet/OZ");
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Torus::ID());
  return TorusPtr(new PrimitivesAPI_Torus(aFeature, aBasePoint, anAxis, theRadius, theRingRadius));
}