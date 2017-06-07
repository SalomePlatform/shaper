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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "FeaturesAPI_Symmetry.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Symmetry::FeaturesAPI_Symmetry(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Symmetry::FeaturesAPI_Symmetry(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::list<ModelHighAPI_Selection>& theMainObjects,
                                       const ModelHighAPI_Selection& theObject)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    GeomAPI_Shape::ShapeType aType = getShapeType(theObject);
    if(aType == GeomAPI_Shape::VERTEX) {
      setPoint(theObject);
    } else if(aType == GeomAPI_Shape::EDGE) {
      setAxis(theObject);
    } else if(aType == GeomAPI_Shape::FACE) {
      setPlane(theObject);
    }
  }
}

//==================================================================================================
FeaturesAPI_Symmetry::~FeaturesAPI_Symmetry()
{
}

//==================================================================================================
void FeaturesAPI_Symmetry::setMainObjects(
  const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mainObjects());

  execute();
}

//==================================================================================================
void FeaturesAPI_Symmetry::setPoint(const ModelHighAPI_Selection& thePointObject)
{
  fillAttribute(FeaturesPlugin_Symmetry::CREATION_METHOD_BY_POINT(), creationMethod());
  fillAttribute(thePointObject, pointObject());

  execute();
}

//==================================================================================================
void FeaturesAPI_Symmetry::setAxis(const ModelHighAPI_Selection& theAxisObject)
{
  fillAttribute(FeaturesPlugin_Symmetry::CREATION_METHOD_BY_AXIS(), creationMethod());
  fillAttribute(theAxisObject, axisObject());

  execute();
}

//==================================================================================================
void FeaturesAPI_Symmetry::setPlane(const ModelHighAPI_Selection& thePlaneObject)
{
  fillAttribute(FeaturesPlugin_Symmetry::CREATION_METHOD_BY_PLANE(), creationMethod());
  fillAttribute(thePlaneObject, planeObject());

  execute();
}

//==================================================================================================
void FeaturesAPI_Symmetry::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Symmetry::OBJECTS_LIST_ID());
  theDumper << aBase << " = model.addSymmetry(" << aDocName << ", " << anAttrObjects;

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Symmetry::CREATION_METHOD())->value();

  if (aCreationMethod == FeaturesPlugin_Symmetry::CREATION_METHOD_BY_POINT()) {
    AttributeSelectionPtr anAttrPoint =
      aBase->selection(FeaturesPlugin_Symmetry::POINT_OBJECT_ID());
    theDumper << ", " << anAttrPoint;
  } else if (aCreationMethod == FeaturesPlugin_Symmetry::CREATION_METHOD_BY_AXIS()) {
    AttributeSelectionPtr anAttrAxis =
      aBase->selection(FeaturesPlugin_Symmetry::AXIS_OBJECT_ID());
    theDumper << ", " << anAttrAxis;
  } else if (aCreationMethod == FeaturesPlugin_Symmetry::CREATION_METHOD_BY_PLANE()) {
    AttributeSelectionPtr anAttrPlane =
      aBase->selection(FeaturesPlugin_Symmetry::PLANE_OBJECT_ID());
    theDumper << ", " << anAttrPlane;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
SymmetryPtr addSymmetry(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theMainObjects,
                              const ModelHighAPI_Selection& theObject)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Symmetry::ID());
  return SymmetryPtr(new FeaturesAPI_Symmetry(aFeature, theMainObjects, theObject));
}
