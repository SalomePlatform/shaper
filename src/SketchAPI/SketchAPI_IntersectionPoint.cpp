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

#include "SketchAPI_IntersectionPoint.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_IntersectionPoint::SketchAPI_IntersectionPoint(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_IntersectionPoint::SketchAPI_IntersectionPoint(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternal )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalLine(theExternal);
  }
}

SketchAPI_IntersectionPoint::SketchAPI_IntersectionPoint(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theExternalName )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalLineName(theExternalName);
  }
}

SketchAPI_IntersectionPoint::~SketchAPI_IntersectionPoint()
{

}

//--------------------------------------------------------------------------------------
void SketchAPI_IntersectionPoint::setByExternalLine(const ModelHighAPI_Selection & theExternalLine)
{
  fillAttribute(theExternalLine, externalFeature());

  execute();
}

void SketchAPI_IntersectionPoint::setByExternalLineName(const std::string & theExternalLineName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalLineName), externalFeature());

  execute();
}

//--------------------------------------------------------------------------------------

void SketchAPI_IntersectionPoint::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr aLine = externalFeature();
  theDumper << aBase << " = " <<
    aSketchName << ".addIntersectionPoint(" << aLine << ")" << std::endl;
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
