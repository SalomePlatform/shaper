// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_PaveFiller.h
// Created:     27 August 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_PaveFiller_H_
#define GeomAlgoAPI_PaveFiller_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_Interface.h>
#include <GeomAPI_DataMapOfShapeShape.h>

/** \class GeomAlgoAPI_PaveFiller
 *  \ingroup DataAlgo
 *  \brief Finds the common parts from the list of shapes and breaks it to shapes with shared subshapes.
 */
class GeomAlgoAPI_PaveFiller : public GeomAPI_Interface
{
public:
  /** \brief Constructor.
   *  \param[in] theListOfShape list of shape which should be splitted.
   *  \param[in] theIsMakeCompSolids if true gather shapes with shared faces to compsolids.
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_PaveFiller(const ListOfShape& theListOfShape, const bool theIsMakeCompSolids = false);

  /// \return true if algorithm succeed.
  GEOMALGOAPI_EXPORT const bool isDone() const;

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// \return result of the boolean algorithm.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Shape> shape() const;

  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_DataMapOfShapeShape> mapOfShapes() const;

  /// \return interface for for History processing.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAlgoAPI_MakeShape> makeShape() const;

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theListOfShape, const bool theIsMakeCompSolids);

private:
  /// Fields.
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> myMap;
  std::shared_ptr<GeomAlgoAPI_MakeShape> myMkShape;
};

#endif
