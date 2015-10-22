// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShapeCustom.cpp
// Created:     4 September 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_MakeShapeCustom.h>

//=================================================================================================
GeomAlgoAPI_MakeShapeCustom::GeomAlgoAPI_MakeShapeCustom()
: GeomAlgoAPI_MakeShape()
{}

//=================================================================================================
void GeomAlgoAPI_MakeShapeCustom::setResult(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  myShape = theShape;
}

//=================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addModified(const std::shared_ptr<GeomAPI_Shape> theBase,
                                              const std::shared_ptr<GeomAPI_Shape> theResult)
{
  return myModified.add(theBase, theResult);
}

//=================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addGenerated(const std::shared_ptr<GeomAPI_Shape> theBase,
                                               const std::shared_ptr<GeomAPI_Shape> theResult)
{
  return myGenerated.add(theBase, theResult);
}

//=================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addDeleted(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  return myDeleted.bind(theShape, theShape);
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_MakeShapeCustom::shape() const
{
  return myShape;
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeCustom::generated(const std::shared_ptr<GeomAPI_Shape> theShape,
                                            ListOfShape& theHistory)
{
  ListOfShape aGenerated;
  if(myGenerated.find(theShape, aGenerated)) {
    theHistory.insert(theHistory.end(), aGenerated.begin(), aGenerated.end());
  }
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeCustom::modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           ListOfShape& theHistory)
{
  ListOfShape aModified;
  if(myModified.find(theShape, aModified)) {
    theHistory.insert(theHistory.end(), aModified.begin(), aModified.end());
  }
}

//=================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  return myDeleted.isBound(theShape);
}