// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        PrimitivesAPI_Cylinder.h
// Created:     12 Jan 2017
// Author:      Clarisse Genrault (CEA)

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
    setObjects(theBasePoint, theAxis);
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
    setObjects(theBasePoint, theAxis);
    setSizes(theRadius, theHeight);
    setAngle(theAngle);
  }
}

//==================================================================================================
void PrimitivesAPI_Cylinder::setObjects(const ModelHighAPI_Selection& theBasePoint,
                                        const ModelHighAPI_Selection& theAxis)
{
  fillAttribute(theBasePoint, basePoint());
  fillAttribute(theAxis, axis());

  execute();
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
void PrimitivesAPI_Cylinder::setAngle(const ModelHighAPI_Double& theAngle)
{
  fillAttribute(theAngle, angle());

  execute();
}

//==================================================================================================
void PrimitivesAPI_Cylinder::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addCylinder(" << aDocName;

  std::string aCreationMethod = aBase->string(PrimitivesPlugin_Cylinder::CREATION_METHOD())->value();
  
  AttributeSelectionPtr anAttrBasePoint =
    aBase->selection(PrimitivesPlugin_Cylinder::BASE_POINT_ID());
  AttributeSelectionPtr anAttrAxis = aBase->selection(PrimitivesPlugin_Cylinder::AXIS_ID());
  AttributeDoublePtr anAttrRadius = aBase->real(PrimitivesPlugin_Cylinder::RADIUS_ID());
  AttributeDoublePtr anAttrHeight = aBase->real(PrimitivesPlugin_Cylinder::HEIGHT_ID());
  
  theDumper << ", " << anAttrBasePoint << ", " << anAttrAxis;
  theDumper << ", " << anAttrRadius << ", " << anAttrHeight;
  
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
                        const ModelHighAPI_Double& theHeight,
                        const ModelHighAPI_Double& theAngle)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cylinder::ID());
  return CylinderPtr(new PrimitivesAPI_Cylinder(aFeature, theBasePoint, theAxis,
                                                theRadius, theHeight, theAngle));
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
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight,
                        const ModelHighAPI_Double& theAngle)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cylinder::ID());
  ModelHighAPI_Selection aBasePoint("VERT", "Origin");
  ModelHighAPI_Selection anAxis("EDGE", "OZ");
  return CylinderPtr(new PrimitivesAPI_Cylinder(aFeature, aBasePoint, anAxis,
                                                theRadius, theHeight, theAngle));
}

//==================================================================================================
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Cylinder::ID());
  ModelHighAPI_Selection aBasePoint("VERT", "Origin");
  ModelHighAPI_Selection anAxis("EDGE", "OZ");
  return CylinderPtr(new PrimitivesAPI_Cylinder(aFeature, aBasePoint, anAxis,
                                                theRadius, theHeight));
}