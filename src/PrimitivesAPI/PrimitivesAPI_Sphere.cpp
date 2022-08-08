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
    fillAttribute(PrimitivesPlugin_Sphere::CREATION_METHOD_BY_PT_RADIUS(), creationMethod());
    fillAttribute(theCenterPoint, centerPoint());
    setRadius(theRadius);
  }
}

//==================================================================================================
PrimitivesAPI_Sphere::PrimitivesAPI_Sphere(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const ModelHighAPI_Double& theRMin,
                                           const ModelHighAPI_Double& theRMax,
                                           const ModelHighAPI_Double& thePhiMin,
                                           const ModelHighAPI_Double& thePhiMax,
                                           const ModelHighAPI_Double& theThetaMin,
                                           const ModelHighAPI_Double& theThetaMax)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(PrimitivesPlugin_Sphere::CREATION_METHOD_BY_DIMENSIONS(), creationMethod());
    fillAttribute(theRMin, rmin());
    fillAttribute(theRMax, rmax());
    fillAttribute(thePhiMin, phimin());
    fillAttribute(thePhiMax, phimax());
    fillAttribute(theThetaMin, thetamin());
    fillAttribute(theThetaMax, thetamax());
    execute();
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
void PrimitivesAPI_Sphere::setRadius(const ModelHighAPI_Double& theRMin,
                                     const ModelHighAPI_Double& theRMax)
{
  fillAttribute(theRMin, rmin());
  fillAttribute(theRMax, rmax());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Sphere::setPhi(const ModelHighAPI_Double& thePhiMin,
                                  const ModelHighAPI_Double& thePhiMax)
{
  fillAttribute(thePhiMin, phimin());
  fillAttribute(thePhiMax, phimax());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Sphere::setTheta(const ModelHighAPI_Double& theThetaMin,
                                    const ModelHighAPI_Double& theThetaMax)
{
  fillAttribute(theThetaMin, thetamin());
  fillAttribute(theThetaMax, thetamax());
  execute();
}

//==================================================================================================
void PrimitivesAPI_Sphere::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addSphere(" << aDocName;

  std::string aCreationMethod = aBase->string(PrimitivesPlugin_Sphere::CREATION_METHOD())->value();

  if (aCreationMethod == PrimitivesPlugin_Sphere::CREATION_METHOD_BY_DIMENSIONS()) {
    AttributeDoublePtr anAttrRMin = aBase->real(PrimitivesPlugin_Sphere::RMIN_ID());
    AttributeDoublePtr anAttrRMax = aBase->real(PrimitivesPlugin_Sphere::RMAX_ID());
    AttributeDoublePtr anAttrPhiMin = aBase->real(PrimitivesPlugin_Sphere::PHIMIN_ID());
    AttributeDoublePtr anAttrPhiMax = aBase->real(PrimitivesPlugin_Sphere::PHIMAX_ID());
    AttributeDoublePtr anAttrThetaMin = aBase->real(PrimitivesPlugin_Sphere::THETAMIN_ID());
    AttributeDoublePtr anAttrThetaMax = aBase->real(PrimitivesPlugin_Sphere::THETAMAX_ID());
    theDumper << ", " << anAttrRMin << ", " << anAttrRMax;
    theDumper << ", " << anAttrPhiMin << ", " << anAttrPhiMax;
    theDumper << ", " << anAttrThetaMin << ", " << anAttrThetaMax;
  } else { // CREATION_METHOD_BY_PT_RADIUS by default to support versions with undefined method
    AttributeSelectionPtr anAttrCenterPoint =
        aBase->selection(PrimitivesPlugin_Sphere::CENTER_POINT_ID());
    AttributeDoublePtr anAttrRadius = aBase->real(PrimitivesPlugin_Sphere::RADIUS_ID());
    theDumper << ", " << anAttrCenterPoint << ", " << anAttrRadius;
  }

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
  ModelHighAPI_Selection aCenterPoint("VERTEX", L"PartSet/Origin");
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Sphere::ID());
  return SpherePtr(new PrimitivesAPI_Sphere(aFeature, aCenterPoint, theRadius));
}

//==================================================================================================
SpherePtr addSphere(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Double& theRMin,
                    const ModelHighAPI_Double& theRMax,
                    const ModelHighAPI_Double& thePhiMin,
                    const ModelHighAPI_Double& thePhiMax,
                    const ModelHighAPI_Double& theThetaMin,
                    const ModelHighAPI_Double& theThetaMax)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Sphere::ID());
  return SpherePtr(new PrimitivesAPI_Sphere(aFeature, theRMin, theRMax, thePhiMin, thePhiMax,
                                            theThetaMin, theThetaMax));
}
