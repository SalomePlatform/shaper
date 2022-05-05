// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include <FeaturesPlugin_Translation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeHierarchy.h>
#include <GeomAPI_Trsf.h>

#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_Transform.h>

#include <FeaturesPlugin_Tools.h>

static const std::string TRANSLATION_VERSION_1("v9.5");

//=================================================================================================
FeaturesPlugin_Translation::FeaturesPlugin_Translation()
{
}

//=================================================================================================
void FeaturesPlugin_Translation::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Translation::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Translation::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Translation::AXIS_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Translation::DISTANCE_ID(),
                       ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Translation::DX_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Translation::DY_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Translation::DZ_ID(),
                       ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Translation::START_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Translation::END_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  if (!aSelection->isInitialized()) {
    // new feature, not read from file
    data()->setVersion(TRANSLATION_VERSION_1);
  }
}

//=================================================================================================
void FeaturesPlugin_Translation::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Translation::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  GeomTrsfPtr aTrsf;
  if (aMethodType == CREATION_METHOD_BY_DISTANCE())
    aTrsf = translationByAxisAndDistance();
  else if (aMethodType == CREATION_METHOD_BY_DIMENSIONS())
    aTrsf = translationByDimensions();
  else if (aMethodType == CREATION_METHOD_BY_TWO_POINTS())
    aTrsf = translationByTwoPoints();

  performTranslation(aTrsf);
}

//=================================================================================================
GeomTrsfPtr FeaturesPlugin_Translation::translationByAxisAndDistance()
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

  std::shared_ptr<GeomAPI_Ax1> anAxis(new GeomAPI_Ax1(anEdge->line()->location(),
                                                      anEdge->line()->direction()));

  // Getting distance.
  double aDistance = real(FeaturesPlugin_Translation::DISTANCE_ID())->value();

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setTranslation(anAxis, aDistance);
  return aTrsf;
}

//=================================================================================================
GeomTrsfPtr FeaturesPlugin_Translation::translationByDimensions()
{
  // Getting dimensions in X, in Y and in Z
  double aDX = real(FeaturesPlugin_Translation::DX_ID())->value();
  double aDY = real(FeaturesPlugin_Translation::DY_ID())->value();
  double aDZ = real(FeaturesPlugin_Translation::DZ_ID())->value();

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setTranslation(aDX, aDY, aDZ);
  return aTrsf;
}

//=================================================================================================
GeomTrsfPtr FeaturesPlugin_Translation::translationByTwoPoints()
{
  // Getting the start point and the end point
  AttributeSelectionPtr aRef1 = data()->selection(FeaturesPlugin_Translation::START_POINT_ID());
  AttributeSelectionPtr aRef2 = data()->selection(FeaturesPlugin_Translation::END_POINT_ID());
  std::shared_ptr<GeomAPI_Pnt> aFirstPoint;
  std::shared_ptr<GeomAPI_Pnt> aSecondPoint;
  if ((aRef1.get() != NULL) && (aRef2.get() != NULL)) {
    GeomShapePtr aShape1 = aRef1->value();
    if (!aShape1.get()) //If we can't get the points directly, try getting them from the context
      aShape1 = aRef1->context()->shape();
    GeomShapePtr aShape2 = aRef2->value();
    if (!aShape2.get())
      aShape2 = aRef2->context()->shape();
    if (aShape1 && aShape2) {
      aFirstPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
      aSecondPoint = GeomAlgoAPI_PointBuilder::point(aShape2);
    }
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  if (aFirstPoint && aSecondPoint) {
    aTrsf->setTranslation(aFirstPoint, aSecondPoint);
  }
  return aTrsf;
}

//=================================================================================================
void FeaturesPlugin_Translation::performTranslation(const GeomTrsfPtr& theTrsf)
{
  if (!theTrsf) {
    setError("Invalid transformation.");
    return;
  }

  bool isKeepSubShapes = data()->version() == TRANSLATION_VERSION_1;

  // Getting objects.
  GeomAPI_ShapeHierarchy anObjects;
  std::list<ResultPtr> aParts;
  ResultPtr aTextureSource;
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECTS_LIST_ID());
  if (!FeaturesPlugin_Tools::shapesFromSelectionList
      (anObjectsSelList, isKeepSubShapes, anObjects, aParts, aTextureSource))
    return;

  std::string anError;
  int aResultIndex = 0;
  // Moving each part.
  for (std::list<ResultPtr>::iterator aPRes = aParts.begin(); aPRes != aParts.end(); ++aPRes) {
    ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPRes);
    ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
    aResultPart->setTrsf(anOrigin, theTrsf);
    setResult(aResultPart, aResultIndex++);
  }

  // Collect transformations for each object in a part.
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);

  for (GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
    anObjectsIt != anObjects.end(); anObjectsIt++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    std::shared_ptr<GeomAlgoAPI_Transform> aTransformAlgo(
        new GeomAlgoAPI_Transform(aBaseShape, theTrsf));

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aTransformAlgo, getKind(), anError)) {
      setError(anError);
      break;
    }

    anObjects.markModified(aBaseShape, aTransformAlgo->shape());
    aMakeShapeList->appendAlgo(aTransformAlgo);
  }

  // Build results of the operation.
  const ListOfShape& anOriginalShapes = anObjects.objects();
  ListOfShape aTopLevel;
  anObjects.topLevelObjects(aTopLevel);
  for (ListOfShape::iterator anIt = aTopLevel.begin(); anIt != aTopLevel.end(); ++anIt) {
    //LoadNamingDS
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    ModelAPI_Tools::loadModifiedShapes(aResultBody, anOriginalShapes, ListOfShape(),
                                       aMakeShapeList, *anIt, "Translated");
    // Copy image data, if any
    ModelAPI_Tools::copyImageAttribute(aTextureSource, aResultBody);
    setResult(aResultBody, aResultIndex++);
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}
