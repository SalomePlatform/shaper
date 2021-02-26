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

#include "SketchAPI_Rectangle.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
#include <ModelHighAPI_Dumper.h>

//--------------------------------------------------------------------------------------
SketchAPI_Rectangle::SketchAPI_Rectangle(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
  : SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Rectangle::SketchAPI_Rectangle(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                                         double theX1, double theY1, double theX2, double theY2,
                                         bool isFirstPointCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByCoordinates(theX1, theY1, theX2, theY2, isFirstPointCenter);
  }
}

SketchAPI_Rectangle::SketchAPI_Rectangle(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                                         const std::shared_ptr<GeomAPI_Pnt2d> & theFirstPoint,
                                         const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint,
                                         bool isFirstPointCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByPoints(theFirstPoint, theEndPoint, isFirstPointCenter);
  }
}

SketchAPI_Rectangle::~SketchAPI_Rectangle()
{
}

//--------------------------------------------------------------------------------------
void SketchAPI_Rectangle::setByCoordinates(
    double theX1, double theY1, double theX2, double theY2, bool isFirstPointCenter)
{
  if(isFirstPointCenter){
    fillAttribute(centerPoint(), theX1, theY1);
    double xStart = 2.0*theX1 - theX2;
    double yStart = 2.0*theY1 - theY2;
    fillAttribute(startPoint(), xStart, yStart);
  }
  else
    fillAttribute(startPoint(), theX1, theY1);

  fillAttribute(endPoint(), theX2, theY2);
  execute(true);
}

void SketchAPI_Rectangle::setByPoints(const std::shared_ptr<GeomAPI_Pnt2d> & theFirstPoint,
                                      const std::shared_ptr<GeomAPI_Pnt2d> & theSecondPoint,
                                      bool isFirstPointCenter)
{  
  if(isFirstPointCenter){
    fillAttribute(theFirstPoint, centerPoint());
    double xStart = 2.0*theFirstPoint->x() - theSecondPoint->x();
    double yStart = 2.0*theFirstPoint->y() - theSecondPoint->y();

    std::shared_ptr<GeomAPI_Pnt2d> theStartPoint = std::make_shared<GeomAPI_Pnt2d>(xStart, yStart);
    fillAttribute(theStartPoint, startPoint());
  }
  else
    fillAttribute(theFirstPoint, startPoint());

  fillAttribute(theSecondPoint, endPoint());
  execute(true);
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

//==================================================================================================
void SketchAPI_Rectangle::dump(ModelHighAPI_Dumper& theDumper) const
{

  FeaturePtr aBase = feature();

  /// do not dump sub-features eg: lines and lines constraints
  AttributeRefListPtr noToDumpList =  aBase->reflist(SketchPlugin_Rectangle::NOT_TO_DUMP_LIST_ID());
  for( int i = 0; i < noToDumpList->size(); i++){
    theDumper.doNotDumpFeature(
          std::dynamic_pointer_cast<ModelAPI_Feature>(noToDumpList->object(i)));
  }

  if (isCopy())
    return; // no need to dump copied feature

  const std::string& aSketchName = theDumper.parentName(aBase);

  FeaturePtr aCenterPointFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(
        aBase->refattr(SketchPlugin_Rectangle::CENTER_REF_ID())->object());
  if(aCenterPointFeature){
    // rectangle has center
    theDumper << aBase << " = " << aSketchName << ".addRectangle("
              << startPoint() << ", " << centerPoint() << ", 1)" << std::endl;
  }
  else
    // rectangle given by start and end points
    theDumper << aBase << " = " << aSketchName << ".addRectangle("
              << startPoint() << ", " << endPoint() << ")" << std::endl;

  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);

}
