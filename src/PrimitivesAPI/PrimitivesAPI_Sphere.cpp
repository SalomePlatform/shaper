// Copyright (C) 2014-201x CEA/DEN, EDF R&D -->

// File:        PrimitivesAPI_Sphere.h
// Created:     16 Mar 2017
// Author:      Clarisse Genrault

#include "PrimitivesAPI_Sphere.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
PrimitivesAPI_Sphere::PrimitivesAPI_Sphere(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
PrimitivesAPI_Sphere::PrimitivesAPI_Sphere(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const ModelHighAPI_Selection& theCenterPoint,
                                           const ModelHighAPI_Double& theRadius)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theCenterPoint, centerPoint());
    setRadius(theRadius);
  }
}

//==================================================================================================
PrimitivesAPI_Sphere::~PrimitivesAPI_Sphere()
{
}

//==================================================================================================
void PrimitivesAPI_Sphere::setCenterPoint(const ModelHighAPI_Selection& theCenterPoint)
{
  fillAttribute(theCenterPoint, centerPoint());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Sphere::setRadius(const ModelHighAPI_Double& theRadius)
{
  fillAttribute(theRadius, radius());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Sphere::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addSphere(" << aDocName;

  AttributeSelectionPtr anAttrCenterPoint =
      aBase->selection(PrimitivesPlugin_Sphere::CENTER_POINT_ID());
  AttributeDoublePtr anAttrRadius = aBase->real(PrimitivesPlugin_Sphere::RADIUS_ID());
  theDumper << ", " << anAttrCenterPoint << ", " << anAttrRadius;

  theDumper << ")" << std::endl;
}

//==================================================================================================
SpherePtr addSphere(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theCenterPoint,
                    const ModelHighAPI_Double& theRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Sphere::ID());
  return SpherePtr(new PrimitivesAPI_Sphere(aFeature, theCenterPoint, theRadius));
}

//==================================================================================================
SpherePtr addSphere(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Double& theRadius)
{
  ModelHighAPI_Selection aCenterPoint("VERTEX", "PartSet/Origin");
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Sphere::ID());
  return SpherePtr(new PrimitivesAPI_Sphere(aFeature, aCenterPoint, theRadius));
}
