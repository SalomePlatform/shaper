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

#include "SketchAPI_Ellipse.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature> & theFeature)
  : SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     double theCenterX, double theCenterY,
                                     double theFocusX, double theFocusY,
                                     double theMinorRadius)
  : SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterFocusAndRadius(theCenterX, theCenterY, theFocusX, theFocusY, theMinorRadius);
  }
}

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                                     double theMinorRadius)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterFocusAndRadius(theCenter, theFocus, theMinorRadius);
  }
}

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Selection& theExternal)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::string& theExternalName)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

SketchAPI_Ellipse::~SketchAPI_Ellipse()
{
}

void SketchAPI_Ellipse::setByCenterFocusAndRadius(double theCenterX, double theCenterY,
                                                  double theFocusX, double theFocusY,
                                                  double theMinorRadius)
{
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(firstFocus(), theFocusX, theFocusY);
  fillAttribute(theMinorRadius, myminorRadius);

  execute();
}

void SketchAPI_Ellipse::setByCenterFocusAndRadius(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                                  const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                                                  double theMinorRadius)
{
  fillAttribute(theCenter, mycenter);
  fillAttribute(theFocus, myfirstFocus);
  fillAttribute(theMinorRadius, myminorRadius);

  execute();
}

void SketchAPI_Ellipse::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());
  execute();
}

void SketchAPI_Ellipse::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());
  execute();
}

void SketchAPI_Ellipse::setCenter(double theX, double theY)
{
  fillAttribute(center(), theX, theY);
  execute();
}

void SketchAPI_Ellipse::setCenter(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter)
{
  fillAttribute(theCenter, mycenter);
  execute();
}

void SketchAPI_Ellipse::setFocus(double theX, double theY)
{
  fillAttribute(firstFocus(), theX, theY);
  execute();
}

void SketchAPI_Ellipse::setFocus(const std::shared_ptr<GeomAPI_Pnt2d> & theFocus)
{
  fillAttribute(theFocus, myfirstFocus);
  execute();
}

void SketchAPI_Ellipse::setMinorRadius(double theMinorRadius)
{
  fillAttribute(theMinorRadius, myminorRadius);
  execute();
}

ModelHighAPI_Selection SketchAPI_Ellipse::majorAxis() const
{
}

ModelHighAPI_Selection SketchAPI_Ellipse::minorAxis() const
{
}

void SketchAPI_Ellipse::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // circle is external
    theDumper << aBase << " = " << aSketchName << ".addEllipse(" << anExternal << ")" << std::endl;
  } else {
    // ellipse given by center, focus and radius
    theDumper << aBase << " = " << aSketchName << ".addEllipse("
              << center() << ", " << firstFocus() << ", " << minorRadius() << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
