// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "SketchAPI_Arc.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintTangent.h>

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             double theCenterX, double theCenterY,
                             double theStartX, double theStartY,
                             double theEndX, double theEndY,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenterX, theCenterY, theStartX,
                        theStartY, theEndX, theEndY, theInversed);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenter, theStart, theEnd, theInversed);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Selection& theExternal)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::string& theExternalName)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

//================================================================================================
SketchAPI_Arc::~SketchAPI_Arc()
{

}

//================================================================================================
void SketchAPI_Arc::setByCenterStartEnd(double theCenterX, double theCenterY,
                                        double theStartX, double theStartY,
                                        double theEndX, double theEndY,
                                        bool theInversed)
{
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByCenterStartEnd(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                        bool theInversed)
{
  fillAttribute(theCenter, mycenter);
  fillAttribute(theStart, mystartPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());

  execute();
}

//===============================================================================================
void SketchAPI_Arc::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());

  execute();
}

//================================================================================================
void SketchAPI_Arc::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // arc is external
    theDumper << aBase << " = " << aSketchName << ".addArc(" << anExternal << ")" << std::endl;
  } else {
    // arc given by center and start, end points
    theDumper << aBase << " = " << aSketchName << ".addArc(" << center() << ", "
              << startPoint() << ", " << endPoint() << ", " << reversed() << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
