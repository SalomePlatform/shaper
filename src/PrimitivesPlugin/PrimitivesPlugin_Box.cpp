// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Box.cpp
// Created:     10 Mar 2016
// Author:      Clarisse Genrault (CEA)

#include <PrimitivesPlugin_Box.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <memory>

//=================================================================================================
PrimitivesPlugin_Box::PrimitivesPlugin_Box() // Nothing to do during instantiation
{
}

//=================================================================================================
void PrimitivesPlugin_Box::initAttributes()
{
  data()->addAttribute(PrimitivesPlugin_Box::CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(PrimitivesPlugin_Box::DX_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Box::DY_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Box::DZ_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(PrimitivesPlugin_Box::POINT_FIRST_ID(), 
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PrimitivesPlugin_Box::POINT_SECOND_ID(),
                       ModelAPI_AttributeSelection::typeId());
}

//=================================================================================================
void PrimitivesPlugin_Box::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(PrimitivesPlugin_Box::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();
  
  if (aMethodType == CREATION_METHOD_BY_DIMENSIONS()) 
    createBoxByDimensions();
  
  if (aMethodType == CREATION_METHOD_BY_TWO_POINTS()) 
    createBoxByTwoPoints();
}

//=================================================================================================
void PrimitivesPlugin_Box::createBoxByDimensions()
{
  double aDx = real(PrimitivesPlugin_Box::DX_ID())->value();
  double aDy = real(PrimitivesPlugin_Box::DY_ID())->value();
  double aDz = real(PrimitivesPlugin_Box::DZ_ID())->value();

  std::shared_ptr<GeomAlgoAPI_Box> aBoxAlgo(new GeomAlgoAPI_Box(aDx,aDy,aDz));
  
  // These checks should be made to the GUI for the feature but 
  // the corresponding validator does not exist yet.
  if (!aBoxAlgo->check()) {
    // The error is not displayed in a popup window. It must be in the status bar.
    setError(aBoxAlgo->getError(), false);
    return;
  }
  
  // Build the box 
  aBoxAlgo->build();

  // Check if the creation of the box
  if(!aBoxAlgo->isDone()) {
    setError(aBoxAlgo->getError());
    return;
  }
  if(!aBoxAlgo->checkValid("Box builder with dimensions")) {
    setError(aBoxAlgo->getError());
    return;
  }
  
  int aResultIndex = 0;
  ResultBodyPtr aResultBox = document()->createBody(data(), aResultIndex); 
  loadNamingDS(aBoxAlgo, aResultBox);
  setResult(aResultBox, aResultIndex);
}

//=================================================================================================
void PrimitivesPlugin_Box::createBoxByTwoPoints() 
{
  AttributeSelectionPtr aRef1 = data()->selection(PrimitivesPlugin_Box::POINT_FIRST_ID());
  AttributeSelectionPtr aRef2 = data()->selection(PrimitivesPlugin_Box::POINT_SECOND_ID());
  
  std::shared_ptr<GeomAlgoAPI_BoxPoints> aBoxAlgo;
  
  if ((aRef1.get() != NULL) && (aRef2.get() != NULL)) {
    GeomShapePtr aShape1 = aRef1->value();
    if (!aShape1.get()) //If we can't get the points directly, try getting them from the context
      aShape1 = aRef1->context()->shape();
    GeomShapePtr aShape2 = aRef2->value();
    if (!aShape2.get())
      aShape2 = aRef2->context()->shape();
    if (aShape1 && aShape2){
      std::shared_ptr<GeomAPI_Pnt> aFirstPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
      std::shared_ptr<GeomAPI_Pnt> aSecondPoint = GeomAlgoAPI_PointBuilder::point(aShape2);
      aBoxAlgo = std::shared_ptr<GeomAlgoAPI_BoxPoints>(
                                  new GeomAlgoAPI_BoxPoints(aFirstPoint,aSecondPoint));
    }
  }
  
  // These checks should be made to the GUI for the feature but 
  // the corresponding validator does not exist yet.
  if (!aBoxAlgo->check()) {
    // The error is not displayed in a popup window. It must be in the message console.
    setError(aBoxAlgo->getError(), false);
    return;
  }
  
  // Build the box 
  aBoxAlgo->build();

  // Check if the creation of the box
  if(!aBoxAlgo->isDone()) {
    // The error is not displayed in a popup window. It must be in the message console.
    setError(aBoxAlgo->getError(), false);
    return;
  }
  if(!aBoxAlgo->checkValid("Box builder with two points")) {
    // The error is not displayed in a popup window. It must be in the message console.
    setError(aBoxAlgo->getError(), false);
    return;
  }
  
  int aResultIndex = 0;
  ResultBodyPtr aResultBox = document()->createBody(data(), aResultIndex); 
  loadNamingDS(aBoxAlgo, aResultBox);
  setResult(aResultBox, aResultIndex);
}

//=================================================================================================
void PrimitivesPlugin_Box::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Box> theBoxAlgo,
                                        std::shared_ptr<ModelAPI_ResultBody> theResultBox)
{
  // Load the result
  theResultBox->store(theBoxAlgo->shape());
  
  // Prepare the naming
  theBoxAlgo->prepareNamingFaces();
  
  // Insert to faces
  int num = 1;
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces = 
    theBoxAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator 
       it=listOfFaces.begin(); it!=listOfFaces.end(); ++it) {
    std::shared_ptr<GeomAPI_Shape> aFace = (*it).second;
    theResultBox->generated(aFace, (*it).first, num++);
  }
}

