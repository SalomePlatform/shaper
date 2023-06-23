// Copyright (C) 2014-2023  CEA, EDF
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

#include "SketchAPI_Circle.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <SketchAPI_Point.h>
#include <SketchPlugin_ConstraintCoincidenceInternal.h>

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   double theCenterX,
                                   double theCenterY,
                                   double theRadius, double theAngle)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterAndRadius(theCenterX, theCenterY, theRadius, theAngle);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                   double theRadius, double theAngle)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterAndRadius(theCenter, theRadius, theAngle);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const ModelHighAPI_Selection& theExternal)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::wstring& theExternalName)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

//==================================================================================================
SketchAPI_Circle::~SketchAPI_Circle()
{

}

// Create point on circle line
void SketchAPI_Circle::createPoint()
{
  // Find sketch
  CompositeFeaturePtr aSketch;
  const std::set<AttributePtr>& aRefs = feature()->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt)
    if ((*anIt)->id() == SketchPlugin_Sketch::FEATURES_ID())
    {
      aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>((*anIt)->owner());
      break;
    }
  if (!aSketch)
    return;

  // create point on line
  FeaturePtr aPointFeature = aSketch->addFeature(SketchPlugin_Point::ID());
  aPointFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(feature());

  AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aPointFeature->attribute(SketchPlugin_Point::COORD_ID()));

  GeomPnt2dPtr aPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    feature()->attribute(SketchPlugin_Circle::ROTATE_ID()))->pnt();

  aCoord->setValue(aPnt);
  aPointFeature->execute();

  FeaturePtr aConstraint = aSketch->addFeature(SketchPlugin_ConstraintCoincidenceInternal::ID());
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
    aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setAttr(feature()->attribute(SketchPlugin_Circle::ROTATE_ID()));

  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
    aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setAttr(aCoord);

  feature()->reference(SketchPlugin_Circle::ROTATE_REF_ID())->setValue(aPointFeature);
}

//==================================================================================================
void SketchAPI_Circle::setByCenterAndRadius(double theCenterX, double theCenterY,
                                            double theRadius, double theAngle)
{
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(theRadius, myradius);
  fillAttribute(theAngle, myangle);

  bool isNeedPoint =
    feature()->integer(SketchPlugin_Circle::VERSION_ID())->value() > SketchPlugin_Circle::THE_VERSION_0;
  if (isNeedPoint)
  {
    fillAttribute(theAngle, angle());

    execute();
    createPoint();
  }
  else
  {
    execute();
  }
}

//==================================================================================================
void SketchAPI_Circle::setByCenterAndRadius(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                            double theRadius, double theAngle)
{
  fillAttribute(theCenter, mycenter);
  fillAttribute(theRadius, myradius);

  bool isNeedPoint =
    feature()->integer(SketchPlugin_Circle::VERSION_ID())->value() > SketchPlugin_Circle::THE_VERSION_0;

  if (isNeedPoint)
  {
    fillAttribute(theAngle, angle());

    execute();
    createPoint();
  }
  else
  {
    execute();
  }
}

//==================================================================================================
void SketchAPI_Circle::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setByExternalName(const std::wstring & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setCenter(double theX, double theY)
{
  fillAttribute(center(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setCenter(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter)
{
  fillAttribute(theCenter, mycenter);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setRadius(double theRadius)
{
  fillAttribute(ModelHighAPI_Double(theRadius), myradius);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setAngle(double theAngle)
{
  fillAttribute(ModelHighAPI_Double(theAngle), myangle);

  execute();
}

//==================================================================================================
// Return created point
std::shared_ptr<SketchAPI_SketchEntity> SketchAPI_Circle::createdPoint() const
{
  std::shared_ptr<SketchAPI_SketchEntity> anEnt;

  AttributeReferencePtr anRef = feature()->reference(SketchPlugin_Circle::ROTATE_REF_ID());
  if (!anRef->isInitialized())
    return anEnt;

  FeaturePtr aFeature = ModelAPI_Feature::feature(anRef->value());
  if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
  {
    anEnt = std::shared_ptr < SketchAPI_SketchEntity>(new SketchAPI_Point(aFeature));
  }
  return anEnt;
}

//==================================================================================================
void SketchAPI_Circle::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  std::string aComName = aBase->integer(SketchPlugin_Circle::VERSION_ID())->value() > SketchPlugin_Circle::THE_VERSION_0 ?
    "addCircleWithPoint" : "addCircle";

  if (anExternal->context()) {
    // circle is external
    theDumper << aBase << " = " << aSketchName << "." << aComName << "(" << anExternal << ")" << std::endl;
  }
  else {// circle given by center and radius
    theDumper << aBase << " = " << aSketchName << "." << aComName << "(" << center() << ", " << radius();
    if (aBase->integer(SketchPlugin_Circle::VERSION_ID())->value() > SketchPlugin_Circle::THE_VERSION_0)
    {
      theDumper << ", " << angle() << ")" << std::endl;
      std::shared_ptr<SketchAPI_SketchEntity> aPoint = createdPoint();
      if (aPoint)
        theDumper << aPoint->feature() << " = " << theDumper.name(aBase) << ".createdPoint()" << std::endl;
    }
    else
    {
      theDumper << ")" << std::endl;
    }
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
