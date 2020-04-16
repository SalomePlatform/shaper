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

#include "FeaturesAPI_Rotation.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Rotation::FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Rotation::FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theMainObjects,
                                           const ModelHighAPI_Selection& theAxisObject,
                                           const ModelHighAPI_Double& theAngle)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theAxisObject, myaxisObject);
    setAngle(theAngle);
  }
}

//==================================================================================================
FeaturesAPI_Rotation::FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theMainObjects,
                                           const ModelHighAPI_Selection& theCenterPoint,
                                           const ModelHighAPI_Selection& theStartPoint,
                                           const ModelHighAPI_Selection& theEndPoint)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    setPoints(theCenterPoint, theStartPoint, theEndPoint);
  }
}

//==================================================================================================
FeaturesAPI_Rotation::~FeaturesAPI_Rotation()
{

}

//==================================================================================================
void FeaturesAPI_Rotation::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::setAxisObject(const ModelHighAPI_Selection& theAxisObject)
{
  fillAttribute(FeaturesPlugin_Rotation::CREATION_METHOD_BY_ANGLE(), mycreationMethod);
  fillAttribute(theAxisObject, myaxisObject);

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::setAngle(const ModelHighAPI_Double& theAngle)
{
  fillAttribute(FeaturesPlugin_Rotation::CREATION_METHOD_BY_ANGLE(), mycreationMethod);
  fillAttribute(theAngle, myangle);

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::setPoints(const ModelHighAPI_Selection& theCenterPoint,
                                     const ModelHighAPI_Selection& theStartPoint,
                                     const ModelHighAPI_Selection& theEndPoint)
{
  fillAttribute(FeaturesPlugin_Rotation::CREATION_METHOD_BY_THREE_POINTS(), mycreationMethod);
  fillAttribute(theCenterPoint, centerPoint());
  fillAttribute(theStartPoint, startPoint());
  fillAttribute(theEndPoint, endPoint());

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Rotation::OBJECTS_LIST_ID());

  theDumper << aBase << " = model.addRotation(" << aDocName << ", " << anAttrObjects;

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Rotation::CREATION_METHOD())->value();

  if (aCreationMethod == FeaturesPlugin_Rotation::CREATION_METHOD_BY_ANGLE()) {
    AttributeSelectionPtr anAttrAxis = aBase->selection(FeaturesPlugin_Rotation::AXIS_OBJECT_ID());
    AttributeDoublePtr anAttrAngle = aBase->real(FeaturesPlugin_Rotation::ANGLE_ID());
    theDumper << ", axis = " << anAttrAxis << ", angle = " << anAttrAngle;
  } else if (aCreationMethod == FeaturesPlugin_Rotation::CREATION_METHOD_BY_THREE_POINTS()) {
    AttributeSelectionPtr anAttrCenterPoint =
      aBase->selection(FeaturesPlugin_Rotation::CENTER_POINT_ID());
    AttributeSelectionPtr anAttrStartPoint =
      aBase->selection(FeaturesPlugin_Rotation::START_POINT_ID());
    AttributeSelectionPtr anAttrEndPoint =
      aBase->selection(FeaturesPlugin_Rotation::END_POINT_ID());
    theDumper << ", center = " << anAttrCenterPoint
              << ", start = " << anAttrStartPoint
              << ", end = " << anAttrEndPoint;
  }

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
RotationPtr addRotation(const std::shared_ptr<ModelAPI_Document>& part,
                        const std::list<ModelHighAPI_Selection>& objects,
                        const ModelHighAPI_Selection& axis,
                        const std::pair<ModelHighAPI_Selection, ModelHighAPI_Double>& angle,
                        const ModelHighAPI_Selection& center,
                        const ModelHighAPI_Selection& start,
                        const ModelHighAPI_Selection& end,
                        const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = part->addFeature(FeaturesAPI_Rotation::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");

  bool byAxis = false;
  bool byPoints = false;
  ModelHighAPI_Selection aPoints[3];
  if (angle.first.variantType() == ModelHighAPI_Selection::VT_Empty) {
    if (axis.variantType() == ModelHighAPI_Selection::VT_Empty) {
      byPoints = true;
      aPoints[0] = center;
      aPoints[1] = start;
      aPoints[2] = end;
    }
    else
      byAxis = true;
  }
  else {
    byPoints = true;
    aPoints[0] = axis;
    aPoints[1] = angle.first;
    aPoints[2] = center;
  }


  RotationPtr aRotation;
  if (byAxis)
    aRotation.reset(new FeaturesAPI_Rotation(aFeature, objects, axis, angle.second));
  else if (byPoints) {
    aRotation.reset(new FeaturesAPI_Rotation(aFeature, objects,
                                             aPoints[0], aPoints[1], aPoints[2]));
  }
  return aRotation;
}
