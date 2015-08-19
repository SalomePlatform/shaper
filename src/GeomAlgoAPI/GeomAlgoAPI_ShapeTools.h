// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeTools.h
// Created:     3 August 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_ShapeTools_H_
#define GeomAlgoAPI_ShapeTools_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>

#include <map>

/** \class GeomAlgoAPI_ShapeTools
 *  \ingroup DataAlgo
 *  \brief
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_ShapeTools
{
public:
  /// \return the total volume of the solids of the current shape or 0.0 if it can be computed.
  static double volume(std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return the centre of mass of the current face. The coordinates returned for the center of mass
  /// are expressed in the absolute Cartesian coordinate system. (This function works only for surfaces).
  static std::shared_ptr<GeomAPI_Pnt> centreOfMass(std::shared_ptr<GeomAPI_Shape> theShape);

  /** \brief Combines faces with common edges to shells, or solids to compsolids.
   *  \param[in] theCompound compound of shapes.
   *  \param[in] theType type of combine.
   *  \param[out] theShells resulting shells.
   *  \param[out] theFreeFaces faces that does not have common edges.
   */
  static void combineShapes(const std::shared_ptr<GeomAPI_Shape> theCompound,
                            const GeomAPI_Shape::ShapeType theType,
                            ListOfShape& theCombinedShapes,
                            ListOfShape& theFreeShapes);
};

#endif
