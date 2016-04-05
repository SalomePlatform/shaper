// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Pipe.h
// Created:     16 March 2016
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Pipe_H_
#define GeomAlgoAPI_Pipe_H_

#include "GeomAlgoAPI.h"

#include "GeomAlgoAPI_MakeSweep.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Pipe
/// \ingroup DataAlgo
/// \brief Allows to create extrusion of objects along a path. It produces the following results from objects:\n
/// Vertex -> Edge\n
/// Edge -> Face\n
/// Wire -> Shell\n
/// Face -> Solid
class GeomAlgoAPI_Pipe : public GeomAlgoAPI_MakeSweep
{
public:
  /// \brief Creates extrusion for the given shape along a path.
  /// \param[in] theBaseShape base shape(vertex, edge, wire of face).
  /// \param[in] thePathShape path shape(edge or wire).
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Pipe(const GeomShapePtr theBaseShape,
                                      const GeomShapePtr thePathShape);

  /// \brief Creates extrusion for the given shape along a path.
  /// \param[in] theBaseShape base shape(vertex, edge, wire of face).
  /// \param[in] thePathShape path shape(edge or wire).
  /// \param[in] theBiNormal edge or wire to preserve the constant angle between the normal vector
  /// to the base object and the BiNormal vector.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Pipe(const GeomShapePtr theBaseShape,
                                      const GeomShapePtr thePathShape,
                                      const GeomShapePtr theBiNormal);

  /// \brief Creates extrusion for the given shape along a path.
  /// \param[in] theBaseShapes base shape(vertex, edge, wire of face).
  /// \param[in] theLocations vertexes on the path. Should be empty or same size as theBaseShapes.
  /// \param[in] thePathShape path shape(edge or wire).
  /// to the base object and the BiNormal vector.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Pipe(const ListOfShape& theBaseShapes,
                                      const ListOfShape& theLocations,
                                      const GeomShapePtr thePathShape);

  /// \return the list of shapes generated from theShape.
  /// \param[in] theShape base shape.
  /// \param[out] theHistory generated shapes.
  GEOMALGOAPI_EXPORT void generated(const GeomShapePtr theShape,
                                    ListOfShape& theHistory);

private:
  void build(const GeomShapePtr theBaseShape,
             const GeomShapePtr thePathShape);

  void build(const GeomShapePtr theBaseShape,
             const GeomShapePtr thePathShape,
             const GeomShapePtr theBiNormal);

  void build(const ListOfShape& theBaseShapes,
             const ListOfShape& theLocations,
             const GeomShapePtr thePathShape);

private:
  //bool myIsPipeShellUsed;
  GeomShapePtr myBaseShape;
  GeomShapePtr myPathShape;
};

#endif
