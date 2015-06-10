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
   *  \param[in] theBasis face or wire to be extruded.
   *  \param[in] theFromShape bottom bounding shape.
   *  \param[in] theFromDistance offset for "from" plane.
   *  \param[in] theToShape top bounding shape.
   *  \param[in] theToDistance offset for "to" plane.
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Prism(const std::shared_ptr<GeomAPI_Shape>& theBasis,
                                       const std::shared_ptr<GeomAPI_Shape>& theFromShape,
                                       double               theFromDistance,
                                       const std::shared_ptr<GeomAPI_Shape>& theToShape,
                                       double               theToDistance);

  /// \return true if algorithm succeed.
  GEOMALGOAPI_EXPORT bool isDone() const;

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT bool isValid() const;

  /// \return true if resulting shape has volume.
  GEOMALGOAPI_EXPORT bool hasVolume() const;

  /// \return result of the Prism algorithm.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Shape> shape() const;

  /// \returns the first shape.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Shape> firstShape() const;

  /// \return the last shape.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Shape> lastShape() const;

  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_DataMapOfShapeShape> mapOfShapes() const;

  /// \return interface for History processing.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAlgoAPI_MakeShape> makeShape() const;

private:
  /// Builds resulting shape.
  void build(const std::shared_ptr<GeomAPI_Shape>& theBasis,
             const std::shared_ptr<GeomAPI_Shape>& theFromShape,
             double               theFromDistance,
             const std::shared_ptr<GeomAPI_Shape>& theToShape,
             double               theToDistance);

private:
  /// Fields.
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_Shape> myFirst;
  std::shared_ptr<GeomAPI_Shape> myLast;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> myMap;
  std::shared_ptr<GeomAlgoAPI_MakeShape> myMkShape;
};

#endif
