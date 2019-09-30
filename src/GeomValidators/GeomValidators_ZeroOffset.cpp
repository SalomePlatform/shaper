// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <GeomValidators_ZeroOffset.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>

//=================================================================================================
bool GeomValidators_ZeroOffset::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                        const std::list<std::string>& theArguments,
                                        Events_InfoMessage& theError) const
{
// LCOV_EXCL_START
  if(theArguments.size() != 9) {
    theError = "Wrong number of validator arguments in xml (expected 9).";
    return false;
  }
// LCOV_EXCL_STOP

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  std::string aSelectedMethod;
  if(theFeature->string(*anIt)) {
    aSelectedMethod = theFeature->string(*anIt)->value();
  }
  anIt++;
  std::string aCreationMethod = *anIt;
  anIt++;

  ListOfShape aFacesList;
  if(theFeature->selection(*anIt)) {
// LCOV_EXCL_START
    AttributeSelectionPtr aFaceSelection = theFeature->selection(*anIt);
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aFaceSelection->context());
    if(aConstruction.get()) {
      int aSketchFacesNum = aConstruction->facesNum();
      for(int aFaceIndex = 0; aFaceIndex < aSketchFacesNum; aFaceIndex++) {
        std::shared_ptr<GeomAPI_Shape> aFace =
          std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
        if(aFace->isFace() && aFace->isPlanar()) {
          aFacesList.push_back(aFace);
        }
      }
    }
// LCOV_EXCL_STOP
  } else if(theFeature->selectionList(*anIt)) {
    AttributeSelectionListPtr aFacesSelectionList = theFeature->selectionList(*anIt);
    for(int anIndex = 0; anIndex < aFacesSelectionList->size(); anIndex++) {
      AttributeSelectionPtr aFaceSel = aFacesSelectionList->value(anIndex);
      std::shared_ptr<GeomAPI_Shape> aFaceShape = aFaceSel->value();
      if(aFaceShape.get() && !aFaceShape->isNull()) { // Getting face.
        if(aFaceShape->isFace() && aFaceShape->isPlanar()) {
          aFacesList.push_back(aFaceShape);
        }
      } else { // This may be the whole sketch result selected, check and get faces.
        ResultPtr aContext = aFaceSel->context();
        std::shared_ptr<GeomAPI_Shape> aContextShape = aContext->shape();
        if(!aContextShape.get()) {
          break;
        }
        ResultConstructionPtr aConstruction =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
        if(!aConstruction.get()) {
          break;
        }
        int aFacesNum = aConstruction->facesNum();
        for(int aFaceIndex = 0; aFaceIndex < aFacesNum || aFacesNum == -1; aFaceIndex++) {
          aFaceShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
          if(aFaceShape->isFace() && aFaceShape->isPlanar()) {
            aFacesList.push_back(aFaceShape);
          }
        }
      }
    }
  }
  anIt++;

  double aToSize = 0.0;
  double aFromSize = 0.0;

  if(theFeature->real(*anIt) && theFeature->real(*anIt)->isInitialized()) {
    aToSize = theFeature->real(*anIt)->value();
  }
  anIt++;
  if(theFeature->real(*anIt) && theFeature->real(*anIt)->isInitialized()) {
    aFromSize = theFeature->real(*anIt)->value();
  }
  anIt++;

  if(aSelectedMethod == aCreationMethod) {
    if(aToSize == -aFromSize) {
      theError = "ToSize = -FromSize.";
      return false;
    } else {
      return true;
    }
  }

  std::shared_ptr<GeomAPI_Shape> aToShape;
  std::shared_ptr<GeomAPI_Shape> aFromShape;

  std::shared_ptr<ModelAPI_AttributeSelection> anAttrSel = theFeature->selection(*anIt);
  if(anAttrSel && anAttrSel->isInitialized()) {
    aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anAttrSel->value());
    if(aToShape.get() == NULL && anAttrSel->context().get() != NULL) {
      aToShape =  anAttrSel->context()->shape();
    }
    if (aToShape->isCompound()) {
      GeomAPI_ShapeIterator anIt(aToShape);
      aToShape = anIt.current();
    }
  }
  anIt++;

  std::shared_ptr<ModelAPI_AttributeDouble> anAttrDouble = theFeature->real(*anIt);
  if(anAttrDouble && anAttrDouble->isInitialized()) {
    aToSize = anAttrDouble->value();
  }
  anIt++;

  anAttrSel = theFeature->selection(*anIt);
  if(anAttrSel && anAttrSel->isInitialized()) {
    aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anAttrSel->value());
    if(aFromShape.get() == NULL && anAttrSel->context().get() != NULL) {
      aFromShape = anAttrSel->context()->shape();
    }
    if (aFromShape->isCompound()) {
      GeomAPI_ShapeIterator anIt(aFromShape);
      aFromShape = anIt.current();
    }
  }
  anIt++;

  anAttrDouble = theFeature->real(*anIt);
  if(anAttrDouble && anAttrDouble->isInitialized()) {
    aFromSize = anAttrDouble->value();
  }

  bool isPlanesCoincident = false;
  if(!aFromShape.get() && !aToShape.get()) {
    isPlanesCoincident = true;
  } else if(aFromShape.get() && aToShape.get()) {
    std::shared_ptr<GeomAPI_Face> aFromFace(new GeomAPI_Face(aFromShape));
    if (aFromFace->isNull()) {
      theError = "From face selection is invalid.";
      return false;
    }
    std::shared_ptr<GeomAPI_Pln>  aFromPln = aFromFace->getPlane();

    std::shared_ptr<GeomAPI_Face> aToFace(new GeomAPI_Face(aToShape));
    if (aToFace->isNull()) {
      theError = "To face selection is invalid.";
      return false;
    }
    std::shared_ptr<GeomAPI_Pln>  aToPln = aToFace->getPlane();

    if(aFromPln.get()) {
      isPlanesCoincident = aFromPln->isCoincident(aToPln);
    }
  } else {
    std::shared_ptr<GeomAPI_Face> aFace;
    if(aFromShape.get()) {
      aFace.reset(new GeomAPI_Face(aFromShape));
      if (aFace->isNull()) {
        theError = "From face selection is invalid.";
        return false;
      }
    } else {
      aFace.reset(new GeomAPI_Face(aToShape));
      if (aFace->isNull()) {
        theError = "To face selection is invalid.";
        return false;
      }
    }
    std::shared_ptr<GeomAPI_Pln> aPln = aFace->getPlane();
    if(aPln.get()) {
      for(ListOfShape::const_iterator
          anIter = aFacesList.cbegin(); anIter != aFacesList.cend(); anIter++) {
        std::shared_ptr<GeomAPI_Shape> aSketchShape = *anIter;
        std::shared_ptr<GeomAPI_Face> aSketchFace(new GeomAPI_Face(aSketchShape));
        std::shared_ptr<GeomAPI_Pln>  aSketchPln = aSketchFace->getPlane();
        if(aPln->isCoincident(aSketchPln)) {
          isPlanesCoincident = true;
          break;
        }
      }
    }
  }

  if(isPlanesCoincident && aFromSize == -aToSize) {
    theError = "FromSize = -ToSize and bounding planes are coincident.";
    return false;
  }

  return true;
}

//=================================================================================================
// LCOV_EXCL_START
bool GeomValidators_ZeroOffset::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  if(theAttribute == "from_object" || theAttribute == "to_object") {
    return true;
  }

  return false;
}
// LCOV_EXCL_STOP
