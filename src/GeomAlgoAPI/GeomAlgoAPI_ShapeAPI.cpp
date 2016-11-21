// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeAPI.cpp
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#include "GeomAlgoAPI_ShapeAPI.h"
#include <GeomAlgoAPI_Box.h>
#include <GeomAlgoAPI_Translation.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Edge.h>
#include <GeomAlgoAPI_EdgeBuilder.h>

#include <iostream>

namespace GeomAlgoAPI_ShapeAPI
{
  //=======================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeBox(
    const double theDx, const double theDy,
    const double theDz) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Box aBoxAlgo(theDx,theDy,theDz);

    if (!aBoxAlgo.check()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }

    aBoxAlgo.build();

    if(!aBoxAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    if (!aBoxAlgo.checkValid("Box builder with dimensions")) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    return aBoxAlgo.shape();
  }

  //======================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeBox(
    std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
    std::shared_ptr<GeomAPI_Pnt> theSecondPoint) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Box aBoxAlgo(theFirstPoint, theSecondPoint);

    if (!aBoxAlgo.check()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }

    aBoxAlgo.build();

    if(!aBoxAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    if (!aBoxAlgo.checkValid("Box builder with two points")) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    return aBoxAlgo.shape();
  }
  
  //=========================================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1>   theAxis,
    const double theDistance) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Translation aTranslationAlgo(theSourceShape, theAxis, theDistance);
    
    if (!aTranslationAlgo.check()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    
    aTranslationAlgo.build();
    
    if(!aTranslationAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    if (!aTranslationAlgo.checkValid("Translation builder with axis and distance")) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    return aTranslationAlgo.shape();
  }
  
  //=========================================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    const double theDx,
    const double theDy,
    const double theDz) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Translation aTranslationAlgo(theSourceShape, theDx, theDy, theDz);
    
    if (!aTranslationAlgo.check()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    
    aTranslationAlgo.build();
    
    if(!aTranslationAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    if (!aTranslationAlgo.checkValid("Translation builder with dimensions")) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    return aTranslationAlgo.shape();
  }
}
