// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "SketchAPI_Rectangle.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Rectangle::SketchAPI_Rectangle(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Rectangle::SketchAPI_Rectangle(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    double theX1, double theY1, double theX2, double theY2)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByCoordinates(theX1, theY1, theX2, theY2);
  }
}

SketchAPI_Rectangle::SketchAPI_Rectangle(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByPoints(theStartPoint, theEndPoint);
  }
}

SketchAPI_Rectangle::~SketchAPI_Rectangle()
{
}

//--------------------------------------------------------------------------------------
void SketchAPI_Rectangle::setByCoordinates(
    double theX1, double theY1, double theX2, double theY2)
{
  fillAttribute(startPoint(), theX1, theY1);
  fillAttribute(endPoint(), theX2, theY2);

  execute();
}

void SketchAPI_Rectangle::setByPoints(
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
{
  fillAttribute(theStartPoint, startPoint());
  fillAttribute(theEndPoint, endPoint());

  execute();
}

//--------------------------------------------------------------------------------------

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_Rectangle::lines() const
{
  std::list<FeaturePtr> aFeatures;
  std::list<ObjectPtr> aList = linesList()->list();
  std::list<ObjectPtr>::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt)
    aFeatures.push_back(ModelAPI_Feature::feature(*anIt));
  return SketchAPI_SketchEntity::wrap(aFeatures);
}
