// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Revolution.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Revolution_H_
#define GeomAlgoAPI_Revolution_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_Ax1.h>
#include <GeomAPI_DataMapOfShapeShape.h>

class gp_Pln;
class gp_Pnt;
class TopoDS_Face;
class TopoDS_Shape;
class TopoDS_Solid;

/** \class GeomAlgoAPI_Revolution
 *  \ingroup DataAlgo
 *  \brief Allows to create the revolution based on a given face, angles and bounding planes.
 *  \n Note that only the planar faces are allowed as bounding faces and resulting
 *  revolution will be bounded by the infinite planes taken from the faces.
 *  \n If the bounding plane was specified with the angle then this plane will be rotated around
 *  the axis to the value of the angle.
 *  \n Note that algorithm return only one solid object. So in case when after cutting with bounding
 *  planes algorithm got more than one solid it will return the closest to the center of mass of
 *  the base face.
 */
class GeomAlgoAPI_Revolution : public GeomAPI_Interface
{
public:
  /** \brief Creates revolution for the given shape.
   *  \param[in] theBasis face for revolution.
   *  \param[in] theFromAngle from angle.
   *  \param[in] theToAngle to angle.
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Revolution(std::shared_ptr<GeomAPI_Shape> theBasis,
                                            std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                            double                         theFromAngle,
                                            double                         theToAngle);

  /** \brief Creates revolution for the given shape.
   *  \param[in] theBasis face for revolution.
   *  \param[in] theFromShape from bounding shape. Can be empty. In this case offset will be applied to the basis.
   *  \param[in] theFromAngle from angle.
   *  \param[in] theToShape to bounding shape. Can be empty. In this case offset will be applied to the basis.
   *  \param[in] theToAngle to angle.
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Revolution(std::shared_ptr<GeomAPI_Shape> theBasis,
                                            std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                            std::shared_ptr<GeomAPI_Shape> theFromShape,
                                            double                         theFromAngle,
                                            std::shared_ptr<GeomAPI_Shape> theToShape,
                                            double                         theToAngle);

  /// \return true if algorithm succeed.
  GEOMALGOAPI_EXPORT const bool isDone() const;

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// \return true if resulting shape has volume.
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// \return result of the Revolution algorithm.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape() const;

  /// \return the first shape.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& firstShape();

  /// \return the last shape.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& lastShape();
 
  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_DataMapOfShapeShape> mapOfShapes() const;

  /// \return interface for History processing.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAlgoAPI_MakeShape> makeShape() const;

private:
  /** \brief Constructs infinite face from thePlane, and with axis located on the same side
   *  of the plane as thePoint. Modifies thePlane axis direction.
   *  \param[in,out] thePlane plane to construct face.
   *  \param[in] thePoint point to locate plane axis.
   *  \return constructed face.
   */
  TopoDS_Face makeFaceFromPlane(gp_Pln& thePlane, const gp_Pnt& thePoint);

  /// \return solid created from face.
  TopoDS_Solid makeSolidFromFace(const TopoDS_Face& theFace);

  /** \brief Selects solid from theShape with closest center of mass to thePoint
   *  \param[in] theShape compound with solids.
   *  \param[in] thePoint point.
   *  \return solid.
   */
  TopoDS_Shape findClosest(const TopoDS_Shape& theShape, const gp_Pnt& thePoint);

  /// Builds resulting shape.
  void build(const std::shared_ptr<GeomAPI_Shape>& theBasis,
             const std::shared_ptr<GeomAPI_Ax1>&   theAxis,
             const std::shared_ptr<GeomAPI_Shape>& theFromShape,
             double                                theFromAngle,
             const std::shared_ptr<GeomAPI_Shape>& theToShape,
             double                                theToAngle);

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