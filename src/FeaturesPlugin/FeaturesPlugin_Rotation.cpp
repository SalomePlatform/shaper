// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <FeaturesPlugin_Rotation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_Rotation.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeHierarchy.h>
#include <GeomAPI_Trsf.h>

#include <FeaturesPlugin_Tools.h>

static const std::string ROTATION_VERSION_1("v9.5");

//=================================================================================================
FeaturesPlugin_Rotation::FeaturesPlugin_Rotation()
{
}

//=================================================================================================
void FeaturesPlugin_Rotation::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Rotation::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Rotation::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Rotation::AXIS_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Rotation::ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Rotation::CENTER_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Rotation::START_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Rotation::END_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  if (!aSelection->isInitialized()) {
    // new feature, not read from file
    data()->setVersion(ROTATION_VERSION_1);
  }
}

//=================================================================================================
void FeaturesPlugin_Rotation::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Rotation::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  GeomTrsfPtr aTrsf;
  if (aMethodType == CREATION_METHOD_BY_ANGLE())
    aTrsf = rotationByAxisAndAngle();
  else if (aMethodType == CREATION_METHOD_BY_THREE_POINTS())
    aTrsf = rotationByThreePoints();

  performRotation(aTrsf);
}

//=================================================================================================
GeomTrsfPtr FeaturesPlugin_Rotation::rotationByAxisAndAngle()
{
  //Getting axis.
  static const std::string aSelectionError = "Error: The axis shape selection is bad.";
  AttributeSelectionPtr anObjRef = selection(AXIS_OBJECT_ID());
  GeomShapePtr aShape = anObjRef->value();
  if (!aShape.get()) {
    if (anObjRef->context().get()) {
      aShape = anObjRef->context()->shape();
    }
  }
  if (!aShape.get()) {
    setError(aSelectionError);
    return GeomTrsfPtr();
  }

  GeomEdgePtr anEdge;
  if (aShape->isEdge())
  {
    anEdge = aShape->edge();
  }
  else if (aShape->isCompound())
  {
    GeomAPI_ShapeIterator anIt(aShape);
    anEdge = anIt.current()->edge();
  }

  if (!anEdge.get())
  {
    setError(aSelectionError);
    return GeomTrsfPtr();
  }

  std::shared_ptr<GeomAPI_Ax1> anAxis (new GeomAPI_Ax1(anEdge->line()->location(),
                                                       anEdge->line()->direction()));
  double anAngle = real(FeaturesPlugin_Rotation::ANGLE_ID())->value();

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf());
  aTrsf->setRotation(anAxis, anAngle);
  return aTrsf;
}

//=================================================================================================
GeomTrsfPtr FeaturesPlugin_Rotation::rotationByThreePoints()
{
  // Getting the center point and two points (start and end)
  std::shared_ptr<GeomAPI_Pnt> aCenterPoint;
  std::shared_ptr<GeomAPI_Pnt> aStartPoint;
  std::shared_ptr<GeomAPI_Pnt> anEndPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> aCenterRef =
    selection(FeaturesPlugin_Rotation::CENTER_POINT_ID());
  std::shared_ptr<ModelAPI_AttributeSelection> aStartPointRef =
    selection(FeaturesPlugin_Rotation::START_POINT_ID());
  std::shared_ptr<ModelAPI_AttributeSelection> anEndPointRef =
    selection(FeaturesPlugin_Rotation::END_POINT_ID());
  if ((aCenterRef.get() != NULL) &&
      (aStartPointRef.get() != NULL) &&
      (anEndPointRef.get() != NULL)) {
    GeomShapePtr aCenterShape = aCenterRef->value();
    if (!aCenterShape.get() && aCenterRef->context().get())
      aCenterShape = aCenterRef->context()->shape();
    GeomShapePtr aStartShape = aStartPointRef->value();
    if (!aStartShape.get() && aStartPointRef->context().get())
      aStartShape = aStartPointRef->context()->shape();
    GeomShapePtr anEndShape = anEndPointRef->value();
    if (!anEndShape.get() && anEndPointRef->context().get())
      anEndShape = anEndPointRef->context()->shape();
    if (aStartShape && anEndShape && aCenterShape) {
      aCenterPoint = GeomAlgoAPI_PointBuilder::point(aCenterShape);
      aStartPoint = GeomAlgoAPI_PointBuilder::point(aStartShape);
      anEndPoint = GeomAlgoAPI_PointBuilder::point(anEndShape);
    }

    if (!aCenterPoint || !aStartPoint || !anEndPoint)
      return GeomTrsfPtr();
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf());
  aTrsf->setRotation(aCenterPoint, aStartPoint, anEndPoint);
  return aTrsf;
}

//=================================================================================================
void FeaturesPlugin_Rotation::performRotation(const GeomTrsfPtr& theTrsf)
{
  if (!theTrsf) {
    setError("Invalid transformation.");
    return;
  }

  bool isKeepSubShapes = data()->version() == ROTATION_VERSION_1;

  // Getting objects.
  GeomAPI_ShapeHierarchy anObjects;
  std::list<ResultPtr> aParts;
  std::string theTextureFile;
  AttributeSelectionListPtr anObjSelList = selectionList(OBJECTS_LIST_ID());
  if (!FeaturesPlugin_Tools::shapesFromSelectionList(
       anObjSelList, isKeepSubShapes, anObjects, aParts, theTextureFile))
    return;

  std::string anError;
  int aResultIndex = 0;
  // Rotating each part.
  for (std::list<ResultPtr>::iterator aPRes = aParts.begin(); aPRes != aParts.end(); ++aPRes) {
    ResultPartPtr anOriginal = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPRes);
    ResultPartPtr aResultPart = document()->copyPart(anOriginal, data(), aResultIndex);
    aResultPart->setTrsf(anOriginal, theTrsf);
    setResult(aResultPart, aResultIndex++);
  }

  // Collect transformations for each object in a part.
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);
  for(GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
      anObjectsIt != anObjects.end(); ++anObjectsIt) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    std::shared_ptr<GeomAlgoAPI_Transform> aRotationAlgo(
        new GeomAlgoAPI_Transform(aBaseShape, theTrsf));

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aRotationAlgo, getKind(), anError)) {
      setError(anError);
      break;
    }

    anObjects.markModified(aBaseShape, aRotationAlgo->shape());
    aMakeShapeList->appendAlgo(aRotationAlgo);
  }

  // Build results of the rotation.
  const ListOfShape& anOriginalShapes = anObjects.objects();
  ListOfShape aTopLevel;
  anObjects.topLevelObjects(aTopLevel);
  for (ListOfShape::iterator anIt = aTopLevel.begin(); anIt != aTopLevel.end(); ++anIt) {
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, anOriginalShapes, ListOfShape(),
                                             aMakeShapeList, *anIt, "Rotated");
    aResultBody->setTextureFile(theTextureFile);
    setResult(aResultBody, aResultIndex++);
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}
