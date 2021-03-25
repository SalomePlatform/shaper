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

#include <FeaturesPlugin_Symmetry.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_Transform.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Ax2.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Trsf.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

#include <FeaturesPlugin_Tools.h>

static const std::string SYMMETRY_VERSION_1("v9.5");

//=================================================================================================
FeaturesPlugin_Symmetry::FeaturesPlugin_Symmetry()
{
}

//=================================================================================================
void FeaturesPlugin_Symmetry::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Symmetry::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Symmetry::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Symmetry::POINT_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Symmetry::AXIS_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Symmetry::PLANE_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Symmetry::KEEP_ORIGINAL_RESULT(),
                       ModelAPI_AttributeBoolean::typeId());

  if (!aSelection->isInitialized()) {
    // new feature, not read from file
    data()->setVersion(SYMMETRY_VERSION_1);
  }
}

//=================================================================================================
void FeaturesPlugin_Symmetry::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Symmetry::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  GeomTrsfPtr aTrsf;
  if (aMethodType == CREATION_METHOD_BY_POINT())
    aTrsf = symmetryByPoint();
  else if (aMethodType == CREATION_METHOD_BY_AXIS())
    aTrsf = symmetryByAxis();
  else if (aMethodType == CREATION_METHOD_BY_PLANE())
    aTrsf = symmetryByPlane();

  performSymmetry(aTrsf);
}

//=================================================================================================
GeomTrsfPtr FeaturesPlugin_Symmetry::symmetryByPoint()
{
  //Getting point.
  std::shared_ptr<GeomAPI_Pnt> aPoint;
  AttributeSelectionPtr anObjRef = selection(FeaturesPlugin_Symmetry::POINT_OBJECT_ID());
  if (anObjRef.get() != NULL) {
    GeomShapePtr aShape1 = anObjRef->value();
    if (!aShape1.get()) {
      aShape1 = anObjRef->context()->shape();
    }
    if (aShape1) {
      aPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
    }
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setSymmetry(aPoint);
  return aTrsf;
}

//=================================================================================================
GeomTrsfPtr FeaturesPlugin_Symmetry::symmetryByAxis()
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
  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setSymmetry(anAxis);
  return aTrsf;
}

//=================================================================================================
GeomTrsfPtr FeaturesPlugin_Symmetry::symmetryByPlane()
{
  //Getting plane.
  static const std::string aSelectionError = "Error: The plane shape selection is bad.";
  AttributeSelectionPtr anObjRef = selection(PLANE_OBJECT_ID());
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

  GeomFacePtr aFace;
  if (aShape->isFace())
  {
    aFace = aShape->face();
  }
  else if (aShape->isCompound())
  {
    GeomAPI_ShapeIterator anIt(aShape);
    aFace = anIt.current()->face();
  }

  if (!aFace.get())
  {
    setError(aSelectionError);
    return GeomTrsfPtr();
  }

  std::shared_ptr<GeomAPI_Ax2> aPlane(new GeomAPI_Ax2(aFace->getPlane()->location(),
                                                      aFace->getPlane()->direction()));
  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setSymmetry(aPlane);
  return aTrsf;
}

//=================================================================================================
void FeaturesPlugin_Symmetry::buildResult(
    const std::shared_ptr<GeomAlgoAPI_MakeShapeList>& theAlgo,
    const std::list<std::shared_ptr<GeomAPI_Shape> >& theOriginalShapes,
    std::shared_ptr<GeomAPI_Shape> theTargetShape, int& theResultIndex)
{
  // Store and name the result.
  ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);
  FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, theOriginalShapes, ListOfShape(),
                                           theAlgo, theTargetShape, "Symmetried");
  setResult(aResultBody, theResultIndex++);
}

//=================================================================================================
void FeaturesPlugin_Symmetry::buildResult(ResultPartPtr theOriginal,
                                          std::shared_ptr<GeomAPI_Trsf> theTrsf,
                                          int& theResultIndex)
{
  if (boolean(KEEP_ORIGINAL_RESULT())->value()) {
    std::shared_ptr<GeomAPI_Trsf> anIdentity(new GeomAPI_Trsf());
    ResultPartPtr aCopy = document()->copyPart(theOriginal, data(), theResultIndex);
    aCopy->setTrsf(theOriginal, anIdentity);
    setResult(aCopy, theResultIndex++);
  }

  ResultPartPtr aResultPart = document()->copyPart(theOriginal, data(), theResultIndex);
  aResultPart->setTrsf(theOriginal, theTrsf);
  setResult(aResultPart, theResultIndex++);
}

//=================================================================================================
static bool performShapeSymmetry(std::shared_ptr<GeomAlgoAPI_MakeShapeList>& theAlgoList,
                                 GeomAPI_ShapeHierarchy& theHierarchy,
                                 GeomShapePtr theBaseShape,
                                 GeomTrsfPtr theTrsf,
                                 bool isKeepOriginalResult,
                                 const std::string& theFeatureKind,
                                 std::string& theError)
{
  std::shared_ptr<GeomAlgoAPI_Transform> aSymmetryAlgo(
      new GeomAlgoAPI_Transform(theBaseShape, theTrsf));

  // Checking that the algorithm worked properly.
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aSymmetryAlgo, theFeatureKind, theError))
    return false;

  theAlgoList->appendAlgo(aSymmetryAlgo);

  // Compose source shape and the result of symmetry.
  GeomShapePtr aCompound;
  if (isKeepOriginalResult) {
    ListOfShape aShapes;
    // add a copy of a base shape otherwise selection of this base shape is bad (2592)
    std::shared_ptr<GeomAlgoAPI_Copy> aCopyAlgo(new GeomAlgoAPI_Copy(theBaseShape));
    aShapes.push_back(aCopyAlgo->shape());
    theAlgoList->appendAlgo(aCopyAlgo);

    aShapes.push_back(aSymmetryAlgo->shape());
    aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  }
  else
    aCompound = aSymmetryAlgo->shape();

  theHierarchy.markModified(theBaseShape, aCompound);
  return true;
}

void FeaturesPlugin_Symmetry::performSymmetry(GeomTrsfPtr theTrsf)
{
  if (!theTrsf) {
    setError("Invalid transformation.");
    return;
  }

  bool isKeepOriginal = boolean(KEEP_ORIGINAL_RESULT())->value();
  bool isKeepSubShapes = data()->version() == SYMMETRY_VERSION_1;

  // Getting objects.
  GeomAPI_ShapeHierarchy anObjects;
  std::list<ResultPtr> aParts;
  AttributeSelectionListPtr anObjSelList = selectionList(OBJECTS_LIST_ID());
  if (!FeaturesPlugin_Tools::shapesFromSelectionList(
       anObjSelList, isKeepSubShapes, anObjects, aParts))
    return;

  std::string anError;
  int aResultIndex = 0;
  // Symmetrying parts.
  for (std::list<ResultPtr>::iterator aPRes = aParts.begin(); aPRes != aParts.end(); ++aPRes) {
    ResultPartPtr anOriginal = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPRes);
    buildResult(anOriginal, theTrsf, aResultIndex);
  }

  // Collect transformations for each object in a part.
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);
  for (GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
       anObjectsIt != anObjects.end(); ++anObjectsIt) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    if (!performShapeSymmetry(aMakeShapeList, anObjects, aBaseShape, theTrsf,
                              isKeepOriginal, getKind(), anError)) {
      setError(anError);
      break;
    }
  }

  // Build results of the rotation.
  const ListOfShape& anOriginalShapes = anObjects.objects();
  ListOfShape aTopLevel;
  anObjects.topLevelObjects(aTopLevel);
  for (ListOfShape::iterator anIt = aTopLevel.begin(); anIt != aTopLevel.end(); ++anIt)
    buildResult(aMakeShapeList, anOriginalShapes, *anIt, aResultIndex);

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}
