// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Rotation.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Rotation_H_
#define GeomAlgoAPI_Rotation_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_Ax1.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <GeomAPI_Shape.h>

/** \class GeomAlgoAPI_Rotation
 *  \ingroup DataAlgo
 *  \brief Creates a copy of the object by rotating it around the axis.
 */
class GeomAlgoAPI_Rotation : public GeomAPI_Interface
{
public:
  /** \brief Creates an object which is obtained from current object by rotating it around the axis.
   *  \param[in] theSourceShape  a shape to be rotated.
   *  \param[in] theAxis         rotation axis.
   *  \param[in] theAngle        rotation angle(in degree).
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                          double                         theAngle);

  /// \return true if algorithm succeed.
  GEOMALGOAPI_EXPORT const bool isDone() const
  { return myDone; }

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// \return true if resulting shape has volume.
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// \return result of the Placement algorithm which may be a Solid or a Face.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape() const;

  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT void  mapOfShapes(GeomAPI_DataMapOfShapeShape& theMap) const;

  /// \return interface for for History processing.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape* makeShape() const;

  /// Destructor.
  GEOMALGOAPI_EXPORT virtual ~GeomAlgoAPI_Rotation();

private:
  /// Builds resulting shape.
  void build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
             std::shared_ptr<GeomAPI_Ax1>   theAxis,
             double                         theAngle);

private:
  /// Fields.
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  GeomAPI_DataMapOfShapeShape    myMap;
  GeomAlgoAPI_MakeShape*         myMkShape;
};

#endif
