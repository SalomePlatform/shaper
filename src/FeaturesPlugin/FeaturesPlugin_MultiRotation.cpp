// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

// File:        FeaturesPlugin_MultiRotation.cpp
// Created:     30 Jan 2017
// Author:      Clarisse Genrault (CEA)

#include <FeaturesPlugin_MultiRotation.h>
#include <FeaturesPlugin_Tools.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_Translation.h>

#include <GeomAPI_ShapeExplorer.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Trsf.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

#include <math.h>
#include <iostream>

//=================================================================================================
FeaturesPlugin_MultiRotation::FeaturesPlugin_MultiRotation()
{
}

//=================================================================================================
void FeaturesPlugin_MultiRotation::initAttributes()
{
  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_MultiRotation::OBJECTS_LIST_ID(),
    ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_MultiRotation::AXIS_ANGULAR_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_MultiRotation::USE_ANGULAR_STEP_ID(),
                       ModelAPI_AttributeString::typeId());
  data()->addAttribute(FeaturesPlugin_MultiRotation::STEP_ANGULAR_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_MultiRotation::NB_COPIES_ANGULAR_ID(),
                       ModelAPI_AttributeInteger::typeId());

#ifdef FEATURE_MULTIROTATION_TWO_DIRECTIONS
  data()->addAttribute(FeaturesPlugin_MultiRotation::USE_RADIAL_DIR_ID(),
                       ModelAPI_AttributeString::typeId());
  data()->addAttribute(FeaturesPlugin_MultiRotation::STEP_RADIAL_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_MultiRotation::NB_COPIES_RADIAL_ID(),
                       ModelAPI_AttributeInteger::typeId());
#endif
}

//=================================================================================================
void FeaturesPlugin_MultiRotation::execute()
{
#ifdef FEATURE_MULTIROTATION_TWO_DIRECTIONS
  std::string useRadialDir = string(FeaturesPlugin_MultiRotation::USE_RADIAL_DIR_ID())->value();
  if (useRadialDir.empty()) {
    performRotation1D();
  } else {
    performRotation2D();
  }
#else
  performRotation1D();
#endif
}

//=================================================================================================
void FeaturesPlugin_MultiRotation::performRotation1D()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_MultiRotation::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    setError("Error: empty selection list");
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) { // may be for not-activated parts
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  //Getting axis.
  static const std::string aSelectionError = "Error: The axis shape selection is bad.";
  AttributeSelectionPtr anObjRef = selection(AXIS_ANGULAR_ID());
  GeomShapePtr aShape = anObjRef->value();
  if (!aShape.get()) {
    if (anObjRef->context().get()) {
      aShape = anObjRef->context()->shape();
    }
  }
  if (!aShape.get()) {
    setError(aSelectionError);
    return;
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
    return;
  }

  std::shared_ptr<GeomAPI_Ax1> anAxis(new GeomAPI_Ax1(anEdge->line()->location(),
                                                      anEdge->line()->direction()));

  // Getting number of copies.
  int nbCopies =
    integer(FeaturesPlugin_MultiRotation::NB_COPIES_ANGULAR_ID())->value();

  if (nbCopies <=0) {
    std::string aFeatureError = "Multirotation builder ";
    aFeatureError+=":: the number of copies for the angular direction is null or negative.";
    setError(aFeatureError);
    return;
  }

  // Getting angle
  double anAngle;
  std::string useAngularStep =
    string(FeaturesPlugin_MultiRotation::USE_ANGULAR_STEP_ID())->value();
  if (!useAngularStep.empty()) {
    anAngle = real(FeaturesPlugin_MultiRotation::STEP_ANGULAR_ID())->value();
  } else {
    anAngle = 360./nbCopies;
  }

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      for (int i=0; i<nbCopies; i++) {
        aTrsf->setRotation(anAxis, i*anAngle);
        ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
        aResultPart->setTrsf(*aContext, aTrsf);
        setResult(aResultPart, aResultIndex);
        aResultIndex++;
      }
    } else {
      std::string anError;
      ListOfShape aListOfShape;
      std::shared_ptr<GeomAlgoAPI_MakeShapeList>
          aListOfRotationAlgo(new GeomAlgoAPI_MakeShapeList);

      for (int i=0; i<nbCopies; i++) {
        std::shared_ptr<GeomAlgoAPI_Rotation> aRotationnAlgo(
          new GeomAlgoAPI_Rotation(aBaseShape, anAxis, i*anAngle));

        // Checking that the algorithm worked properly.
        if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aRotationnAlgo, getKind(), anError)) {
          setError(anError);
          break;
        }
        aListOfShape.push_back(aRotationnAlgo->shape());
        aListOfRotationAlgo->appendAlgo(aRotationnAlgo);
      }
      std::shared_ptr<GeomAPI_Shape> aCompound =
        GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

      ListOfShape aBaseShapes;
      aBaseShapes.push_back(aBaseShape);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aBaseShapes, ListOfShape(),
                                               aListOfRotationAlgo, aCompound, "Rotated");

      setResult(aResultBody, aResultIndex);
      aResultIndex++;
    }
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
#ifdef FEATURE_MULTIROTATION_TWO_DIRECTIONS
void FeaturesPlugin_MultiRotation::performRotation2D()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_MultiRotation::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) { // may be for not-activated parts
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  //Getting axis.
  std::shared_ptr<GeomAPI_Ax1> anAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_MultiRotation::AXIS_ANGULAR_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
  } else if (anObjRef && !anObjRef->value() && anObjRef->context() &&
             anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->context()->shape()));
  }
  if(anEdge) {
    anAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(),
                                                          anEdge->line()->direction()));
  }

  // Getting number of copies int he angular direction.
  int nbAngular =
    integer(FeaturesPlugin_MultiRotation::NB_COPIES_ANGULAR_ID())->value();

  if (nbAngular <=0) {
    std::string aFeatureError = "Multirotation builder ";
    aFeatureError+=":: the number of copies for the angular direction is null or negative.";
    setError(aFeatureError);
    return;
  }

  // Getting number of copies int he radial direction.
  int nbRadial =
    integer(FeaturesPlugin_MultiRotation::NB_COPIES_RADIAL_ID())->value();

  if (nbRadial <=0) {
    std::string aFeatureError = "Multirotation builder ";
    aFeatureError+=":: the number of copies for the radial direction is null or negative.";
    setError(aFeatureError);
    return;
  }

  // Getting angle
  double anAngle;
  std::string useAngularStep =
    string(FeaturesPlugin_MultiRotation::USE_ANGULAR_STEP_ID())->value();
  if (!useAngularStep.empty()) {
    anAngle = real(FeaturesPlugin_MultiRotation::STEP_ANGULAR_ID())->value();
  } else {
    anAngle = 360./nbAngular;
  }

  // Getting step
  double aStep = real(FeaturesPlugin_MultiRotation::STEP_RADIAL_ID())->value();

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    std::shared_ptr<GeomAPI_Dir> aDir =
      GeomAlgoAPI_ShapeTools::buildDirFromAxisAndShape(aBaseShape, anAxis);
    double x = aDir->x();
    double y = aDir->y();
    double z = aDir->z();
    double norm = sqrt(x*x+y*y+z*z);

    // Setting result.
    if (isPart) {
      /*ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      for (int j=0; j<aSecondNbCopies; j++) {
        for (int i=0; i<aFirstNbCopies; i++) {
          double dx = i*aFirstStep*x1/norm1+j*aSecondStep*x2/norm2;
          double dy = i*aFirstStep*y1/norm1+j*aSecondStep*y2/norm2;
          double dz = i*aFirstStep*z1/norm1+j*aSecondStep*z2/norm2;
          aTrsf->setTranslation(dx, dy, dz);
          ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
          aResultPart->setTrsf(*aContext, aTrsf);
          setResult(aResultPart, aResultIndex);
          aResultIndex++;
        }
      }*/
    } else {
      ListOfShape aListOfShape;
      std::list<std::shared_ptr<GeomAlgoAPI_Translation> > aListOfTranslationAlgo;
      std::list<std::shared_ptr<GeomAlgoAPI_Rotation> > aListOfRotationAlgo;
      for (int j=0; j<nbRadial; j++) {
        // Translation
        double dx = j*aStep*x/norm;
        double dy = j*aStep*y/norm;
        double dz = j*aStep*z/norm;
        std::shared_ptr<GeomAlgoAPI_Translation> aTranslationAlgo(
          new GeomAlgoAPI_Translation(aBaseShape, dx, dy, dz));

        if (!aTranslationAlgo->check()) {
          setError(aTranslationAlgo->getError());
          break;
        }

        aTranslationAlgo->build();

        // Checking that the algorithm worked properly.
        if (!aTranslationAlgo->isDone()) {
          static const std::string aFeatureError = "Error : Multirotation algorithm failed.";
          setError(aFeatureError);
          break;
        }
        if (aTranslationAlgo->shape()->isNull()) {
          static const std::string aShapeError = "Error : Resulting shape is null.";
          setError(aShapeError);
          break;
        }
        if (!aTranslationAlgo->isValid()) {
          static const std::string aFeatureError = "Error : Resulting shape in not valid.";
          setError(aFeatureError);
          break;
        }
        aListOfShape.push_back(aTranslationAlgo->shape());
        aListOfTranslationAlgo.push_back(aTranslationAlgo);
        for (int i=1; i<nbAngular; i++) {
          std::shared_ptr<GeomAlgoAPI_Rotation> aRotationnAlgo(
            new GeomAlgoAPI_Rotation(aTranslationAlgo->shape(), anAxis, i*anAngle));
          if (!aRotationnAlgo->check()) {
            setError(aTranslationAlgo->getError());
            break;
          }
          aRotationnAlgo->build();// Checking that the algorithm worked properly.
          if (!aRotationnAlgo->isDone()) {
            static const std::string aFeatureError = "Error : Multirotation algorithm failed.";
            setError(aFeatureError);
            break;
          }
          if (aRotationnAlgo->shape()->isNull()) {
            static const std::string aShapeError = "Error : Resulting shape is null.";
            setError(aShapeError);
            break;
          }
          if (!aRotationnAlgo->isValid()) {
            static const std::string aFeatureError = "Error : Resulting shape in not valid.";
            setError(aFeatureError);
            break;
          }
          aListOfShape.push_back(aRotationnAlgo->shape());
          aListOfRotationAlgo.push_back(aRotationnAlgo);
        }
      }
      std::shared_ptr<GeomAPI_Shape> aCompound =
        GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      aResultBody->storeModified(aBaseShape, aCompound);

      loadNamingDS2(aListOfTranslationAlgo, aResultBody, aBaseShape);
      loadNamingDS3(aListOfRotationAlgo, aResultBody, aBaseShape, nbRadial);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;
    }
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_MultiRotation::loadNamingDS2(
    std::list<std::shared_ptr<GeomAlgoAPI_Translation> > theListOfTranslationAlgo,
    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
    std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  for (std::list<std::shared_ptr<GeomAlgoAPI_Translation> >::const_iterator anIt =
    theListOfTranslationAlgo.begin(); anIt != theListOfTranslationAlgo.cend(); ++anIt) {
    // naming of faces
    theResultBody->loadModifiedShapes(*anIt, theBaseShape, GeomAPI_Shape::FACE, "Rotated_Face");

    // naming of edges
    theResultBody->loadModifiedShapes(*anIt, theBaseShape, GeomAPI_Shape::EDGE, "Rotated_Edge");

    // naming of vertex
    theResultBody->loadModifiedShapes(*anIt, theBaseShape, GeomAPI_Shape::VERTEX, "Rotated_Vertex");
  }
}

//=================================================================================================
void FeaturesPlugin_MultiRotation::loadNamingDS3(
    std::list<std::shared_ptr<GeomAlgoAPI_Rotation> > theListOfRotationAlgo,
    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
    std::shared_ptr<GeomAPI_Shape> theBaseShape, int nb)
{
  int anIndex = nb+1;
  std::string aRotatedName;

  for (std::list<std::shared_ptr<GeomAlgoAPI_Rotation> >::const_iterator anIt =
    theListOfRotationAlgo.begin(); anIt != theListOfRotationAlgo.cend(); ++anIt) {

    // naming of faces
    int numFace = 1;
    GeomAPI_ShapeExplorer anExp((*anIt)->shape(), GeomAPI_Shape::FACE);
    for(; anExp.more(); anExp.next()) {
       aRotatedName = "Rotated_Face_" + std::to_string((long long) anIndex);
       aRotatedName = aRotatedName + "_" + std::to_string((long long) numFace);
       theResultBody->generated(anExp.current(), aRotatedName);
       ++numFace;
    }
    ++anIndex;
  }
}
#endif
