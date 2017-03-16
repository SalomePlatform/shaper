// Copyright (C) 2014-201x CEA/DEN, EDF R&D -->

// File:        PrimitivesAPI_Cylinder.cpp
// Created:     12 Jan 2017
// Author:      Clarisse Genrault

#include "PrimitivesAPI_Cylinder.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
PrimitivesAPI_Cylinder::PrimitivesAPI_Cylinder(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
PrimitivesAPI_Cylinder::PrimitivesAPI_Cylinder(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const ModelHighAPI_Selection& theBasePoint,
                                               const ModelHighAPI_Selection& theAxis,
                                               const ModelHighAPI_Double& theRadius,
                                               const ModelHighAPI_Double& theHeight)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(PrimitivesPlugin_Cylinder::CREATION_METHOD_CYLINDER(), creationMethod());
    fillAttribute(theBasePoint, basePoint());
    fillAttribute(theAxis, axis());
    setSizes(theRadius, theHeight);
  }
}

//==================================================================================================
PrimitivesAPI_Cylinder::PrimitivesAPI_Cylinder(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const ModelHighAPI_Selection& theBasePoint,
                                               const ModelHighAPI_Selection& theAxis,
                                               const ModelHighAPI_Double& theRadius,
                                               const ModelHighAPI_Double& theHeight,
                                               const ModelHighAPI_Double& theAngle)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(PrimitivesPlugin_Cylinder::CREATION_METHOD_CYLINDER_PORTION(), creationMethod());
    fillAttribute(theBasePoint, basePoint());
    fillAttribute(theAxis, axis());
    fillAttribute(theAngle, angle());
    setSizes(theRadius, theHeight);
  }
}

//==================================================================================================
PrimitivesAPI_Cylinder::~PrimitivesAPI_Cylinder()
{

}

//==================================================================================================
void PrimitivesAPI_Cylinder::setSizes(const ModelHighAPI_Double& theRadius,
                                      const ModelHighAPI_Double& theHeight)
{
  fillAttribute(theRadius, radius());
  fillAttribute(theHeight, height());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Cylinder::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addCylinder(" << aDocName;

  AttributeSelectionPtr anAttrBasePoint =
    aBase->selection(PrimitivesPlugin_Cylinder::BASE_POINT_ID());
  AttributeSelectionPtr anAttrAxis = aBase->selection(PrimitivesPlugin_Cylinder::AXIS_ID());
  theDumper << ", " << anAttrBasePoint << ", " << anAttrAxis;

  AttributeDoublePtr anAttrRadius = aBase->real(PrimitivesPlugin_Cylinder::RADIUS_ID());
  AttributeDoublePtr anAttrHeight = aBase->real(PrimitivesPlugin_Cylinder::HEIGHT_ID());
  theDumper << ", " << anAttrRadius << ", " << anAttrHeight;

  std::string aCreationMethod =
    aBase->string(PrimitivesPlugin_Cylinder::CREATION_METHOD())->value();

  if (aCreationMethod == PrimitivesPlugin_Cylinder::CREATION_METHOD_CYLINDER_PORTION()) {
    AttributeDoublePtr anAttrAngle = aBase->real(PrimitivesPlugin_Cylinder::ANGLE_ID());
    theDumper << ", " << anAttrAngle;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Selection& theBasePoint,
                        const ModelHighAPI_Selection& theAxis,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cylinder::ID());
  return CylinderPtr(new PrimitivesAPI_Cylinder(aFeature, theBasePoint, theAxis,
                                                theRadius, theHeight));
}

//==================================================================================================
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Selection& theBasePoint,
                        const ModelHighAPI_Selection& theAxis,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight,
                        const ModelHighAPI_Double& theAngle)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cylinder::ID());
  return CylinderPtr(new PrimitivesAPI_Cylinder(aFeature, theBasePoint, theAxis,
                                                theRadius, theHeight, theAngle));
}

//==================================================================================================
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight)
{
  ModelHighAPI_Selection aBasePoint("VERTEX", "PartSet/Origin");
  ModelHighAPI_Selection anAxis("EDGE", "PartSet/OZ");
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cylinder::ID());
  return CylinderPtr(new PrimitivesAPI_Cylinder(aFeature, aBasePoint, anAxis,
                                                theRadius, theHeight));
}

//==================================================================================================
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight,
                        const ModelHighAPI_Double& theAngle)
{
  ModelHighAPI_Selection aBasePoint("VERTEX", "PartSet/Origin");
  ModelHighAPI_Selection anAxis("EDGE", "PartSet/OZ");
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cylinder::ID());
  return CylinderPtr(new PrimitivesAPI_Cylinder(aFeature, aBasePoint, anAxis,
                                                theRadius, theHeight, theAngle));
}
