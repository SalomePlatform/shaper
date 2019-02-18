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

#include "SketchAPI_Point.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    double theX, double theY)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setCoordinates(theX, theY);
  }
}

SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::shared_ptr<GeomAPI_Pnt2d> & thePoint)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setCoordinates(thePoint);
  }
}

SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternal )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theExternalName )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

SketchAPI_Point::~SketchAPI_Point()
{

}

//--------------------------------------------------------------------------------------
void SketchAPI_Point::setCoordinates(
    double theX, double theY)
{
  fillAttribute(coordinates(), theX, theY);

  execute();
}

void SketchAPI_Point::setCoordinates(
    const std::shared_ptr<GeomAPI_Pnt2d> & thePoint)
{
  fillAttribute(thePoint, coordinates());

  execute();
}

void SketchAPI_Point::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());

  execute();
}

void SketchAPI_Point::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("VERTEX", theExternalName), external());

  execute();
}

//--------------------------------------------------------------------------------------

void SketchAPI_Point::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // point is external
    theDumper << aBase << " = " << aSketchName << ".addPoint(" << anExternal << ")" << std::endl;
  } else {
    // point given by coordinates
    std::shared_ptr<GeomDataAPI_Point2D> aPoint = coordinates();
    theDumper << aBase << " = " << aSketchName << ".addPoint(" << aPoint << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
