// Copyright (C) 2014-201x CEA/DEN, EDF R&D -->

// File:        PrimitivesAPI_Cone.cpp
// Created:     20 Mar 2017
// Author:      Clarisse Genrault

#include "PrimitivesAPI_Cone.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
PrimitivesAPI_Cone::PrimitivesAPI_Cone(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
PrimitivesAPI_Cone::PrimitivesAPI_Cone(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBasePoint,
                                       const ModelHighAPI_Selection& theAxis,
                                       const ModelHighAPI_Double& theBaseRadius,
                                       const ModelHighAPI_Double& theTopRadius,
                                       const ModelHighAPI_Double& theHeight)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theBasePoint, basePoint());
    fillAttribute(theAxis, axis());
    fillAttribute(theBaseRadius, baseRadius());
    fillAttribute(theTopRadius, topRadius());
    setHeight(theHeight);
  }
}

//==================================================================================================
PrimitivesAPI_Cone::~PrimitivesAPI_Cone()
{
}

//==================================================================================================
void PrimitivesAPI_Cone::setRadius(const ModelHighAPI_Double& theBaseRadius,
                                   const ModelHighAPI_Double& theTopRadius)
{
  fillAttribute(theBaseRadius, baseRadius());
  fillAttribute(theTopRadius, topRadius());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Cone::setHeight(const ModelHighAPI_Double& theHeight)
{
  fillAttribute(theHeight, height());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Cone::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addCone(" << aDocName;

  AttributeSelectionPtr anAttrBasePoint =
      aBase->selection(PrimitivesPlugin_Cone::BASE_POINT_ID());
  AttributeSelectionPtr anAttrAxis =
      aBase->selection(PrimitivesPlugin_Cone::AXIS_ID());
  theDumper << ", " << anAttrBasePoint << ", " << anAttrAxis;

  AttributeDoublePtr anAttrBaseRadius = aBase->real(PrimitivesPlugin_Cone::BASE_RADIUS_ID());
  AttributeDoublePtr anAttrTopRadius = aBase->real(PrimitivesPlugin_Cone::TOP_RADIUS_ID());
  AttributeDoublePtr anAttrHeight = aBase->real(PrimitivesPlugin_Cone::HEIGHT_ID());
  theDumper << ", " << anAttrBaseRadius << ", " << anAttrTopRadius << ", " << anAttrHeight;

  theDumper << ")" << std::endl;
}

//==================================================================================================
ConePtr addCone(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& theBasePoint,
                const ModelHighAPI_Selection& theAxis,
                const ModelHighAPI_Double& theBaseRadius,
                const ModelHighAPI_Double& theTopRadius,
                const ModelHighAPI_Double& theHeight)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cone::ID());
  return ConePtr(new PrimitivesAPI_Cone(aFeature, theBasePoint, theAxis, theBaseRadius,
                                        theTopRadius, theHeight));
}

//==================================================================================================
ConePtr addCone(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Double& theBaseRadius,
                const ModelHighAPI_Double& theTopRadius,
                const ModelHighAPI_Double& theHeight)
{
  ModelHighAPI_Selection aBasePoint("VERTEX", "PartSet/Origin");
  ModelHighAPI_Selection anAxis("EDGE", "PartSet/OZ");
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cone::ID());
  return ConePtr(new PrimitivesAPI_Cone(aFeature, aBasePoint, anAxis, theBaseRadius,
                                        theTopRadius, theHeight));
}