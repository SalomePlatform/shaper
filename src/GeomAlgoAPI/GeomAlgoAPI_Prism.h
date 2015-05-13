// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Prism.h
// Created:     5 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Prism_H_
#define GeomAlgoAPI_Prism_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <memory>

/** \class GeomAlgoAPI_Prism
 *  \ingroup DataAlgo
 *  \brief Allows to create the prism based on a given face and bounding planes.
 *  \n Note that only planar faces are allowed as bounding faces and resulting
 *  extrusion will be bounded by the infinite planes taken from the faces.
 */
class GeomAlgoAPI_Prism : public GeomAPI_Interface
{
public:
  /** \brief Creates extrusion for the given shape along the normal for this shape.
   *  \param[in] theBasis face or wire to be extruded;
   *  \param[in] theFromShape bottom bounding shape;
   *  \param[in] theToShape top bounding shape;
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Prism(std::shared_ptr<GeomAPI_Shape> theBasis,
                                       std::shared_ptr<GeomAPI_Shape> theFromShape,
                                       std::shared_ptr<GeomAPI_Shape> theToShape);

  /// \return true if algorithm succeed.
  GEOMALGOAPI_EXPORT const bool isDone() const;

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// \return true if resulting shape has volume.
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// \return result of the Prism algorithm.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape() const;

  /// \returns the first shape.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& firstShape();

  /// \return the last shape.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& lastShape();

  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT void  mapOfShapes(GeomAPI_DataMapOfShapeShape& theMap) const;

  /// \return interface for History processing.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape* makeShape() const;

  /// Destructor.
  GEOMALGOAPI_EXPORT  ~GeomAlgoAPI_Prism();

private:
  /// Builds resulting shape.
  void build(const std::shared_ptr<GeomAPI_Shape>& theBasis);

private:
  /// Fields.
  std::shared_ptr<GeomAPI_Shape> myFromShape;
  std::shared_ptr<GeomAPI_Shape> myToShape;
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_Shape> myFirst;
  std::shared_ptr<GeomAPI_Shape> myLast;
  GeomAPI_DataMapOfShapeShape myMap;
  GeomAlgoAPI_MakeShape* myMkShape;
};

#endif
