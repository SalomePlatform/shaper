// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Transform.h
// Created:     29 July 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Transform_H_
#define GeomAlgoAPI_Transform_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Trsf.h>

/** \class GeomAlgoAPI_Transform
 *  \ingroup DataAlgo
 *  \brief Creates a copy of the object by transformating it.
 */
class GeomAlgoAPI_Transform : public GeomAPI_Interface
{
public:
  /** \brief Creates an object which is obtained from current object by transformating it.
   *  \param[in] theSourceShape  a shape to be transformed.
   *  \param[in] theTrsf         transformation.
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Transform(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Trsf>  theTrsf);

  /// \return true if algorithm succeed.
  GEOMALGOAPI_EXPORT const bool isDone() const
  { return myDone; }

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// \return true if resulting shape has volume.
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// \return result of the transformation algorithm.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape() const;

  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_DataMapOfShapeShape> mapOfShapes() const;

  /// \return interface for for History processing.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAlgoAPI_MakeShape> makeShape() const;

  /// \return the transformation.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Trsf> transformation() const;

private:
  /// Builds resulting shape.
  void build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
             std::shared_ptr<GeomAPI_Trsf>  theTrsf);

private:
  /// Fields.
  bool myDone;
  std::shared_ptr<GeomAPI_Trsf> myTrsf;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> myMap;
  std::shared_ptr<GeomAlgoAPI_MakeShape> myMkShape;
};

#endif
