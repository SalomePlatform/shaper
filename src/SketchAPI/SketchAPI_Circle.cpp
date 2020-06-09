// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
                                   double theRadius)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterAndRadius(theCenterX, theCenterY, theRadius);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                   double theRadius)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterAndRadius(theCenter, theRadius);
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

//==================================================================================================
void SketchAPI_Circle::setByCenterAndRadius(double theCenterX, double theCenterY, double theRadius)
{
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(theRadius, myradius);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setByCenterAndRadius(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                            double theRadius)
{
  fillAttribute(theCenter, mycenter);
  fillAttribute(theRadius, myradius);

  execute();
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
void SketchAPI_Circle::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // circle is external
    theDumper << aBase << " = " << aSketchName << ".addCircle(" << anExternal << ")" << std::endl;
  } else {
    // circle given by center and radius
    theDumper << aBase << " = " << aSketchName << ".addCircle("
              << center() << ", " << radius() << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
