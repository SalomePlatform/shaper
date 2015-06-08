// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Movement.h
// Created:     8 June 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Movement_H_
#define GeomAlgoAPI_Movement_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_Ax1.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <GeomAPI_Shape.h>

/** \class GeomAlgoAPI_Movement
 *  \ingroup DataAlgo
 *  \brief Creates a copy of the object by moving it along the axis.
 */
class GeomAlgoAPI_Movement : public GeomAPI_Interface
{
public:
  /** \brief Creates an object which is obtained from current object by moving it along the axis.
   *  \param[in] theSourceShape  a shape to be moved.
   *  \param[in] theAxis         movement axis.
   *  \param[in] theDistance     movement distance.
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Movement(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                          double                         theDistance);

  /// \return true if algorithm succeed.
  GEOMALGOAPI_EXPORT const bool isDone() const
  { return myDone; }

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// \return true if resulting shape has volume.
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// \return result of the movement algorithm.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape() const;

  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_DataMapOfShapeShape> mapOfShapes() const;

  /// \return interface for for History processing.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAlgoAPI_MakeShape> makeShape() const;

private:
  /// Builds resulting shape.
  void build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
             std::shared_ptr<GeomAPI_Ax1>   theAxis,
             double                         theDistance);

private:
  /// Fields.
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> myMap;
  std::shared_ptr<GeomAlgoAPI_MakeShape> myMkShape;
};

#endif
