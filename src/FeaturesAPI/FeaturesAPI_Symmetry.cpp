// Copyright (C) 2014-2016 CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Symmetry.cpp
// Created:     07 Dec 2016
// Author:      Clarisse Genrault (CEA)

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
