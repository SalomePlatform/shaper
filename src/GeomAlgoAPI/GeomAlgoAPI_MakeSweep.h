// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeSweep.h
// Created:     23 November 2015
// Author:      Dmitry Bobylev
#ifndef GeomAlgoAPI_MakeSweep_H_
#define GeomAlgoAPI_MakeSweep_H_

#include <GeomAlgoAPI.h>

#include <GeomAlgoAPI_MakeShapeList.h>

/**\class GeomAlgoAPI_MakeSweep
 * \ingroup DataAlgo
 * \brief The abstract class MakeSweep is the root class of swept primitives.
 * Sweeps are objects you obtain by sweeping a profile along a path.
 * The profile can be any topology and the path is usually a curve or
 * a wire. The profile generates objects according to the following rules:
 * - Vertices generate Edges.
 * - Edges generate Faces.
 * - Wires generate Shells.
 * - Faces generate Solids.
 * - Shells generate Composite Solids.
 * You are not allowed to sweep Solids and Composite Solids.
 */
class GeomAlgoAPI_MakeSweep : public GeomAlgoAPI_MakeShapeList
{
public:
  /// \returns the list of from shapes.
  GEOMALGOAPI_EXPORT virtual const ListOfShape& fromShapes() const;

  /// \return the list of to shapes.
  GEOMALGOAPI_EXPORT virtual const ListOfShape& toShapes() const;

protected:
  /// Empty constructor.
  GeomAlgoAPI_MakeSweep() : GeomAlgoAPI_MakeShapeList() {};

  /// \brief Adds a shape to list of from shape.
  /// \param[in] theShape a shape to add.
  void addFromShape(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \brief Sets from shapes
  /// \param[in] theListOfShapes list of from shapes.
  void setFromShapes(const ListOfShape& theListOfShapes);

  /// \brief Adds a face to list of to shape.
  /// \param[in] theShape a face to add.
  void addToShape(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \brief Sets to shapes
  /// \param[in] theListOfShapes list of to shapes.
  void setToShapes(const ListOfShape& theListOfShapes);

private:
  ListOfShape myFromShapes;
  ListOfShape myToShapes;
};

#endif
