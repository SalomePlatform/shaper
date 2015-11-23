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
  /// \returns the list of from faces.
  GEOMALGOAPI_EXPORT const ListOfShape& fromFaces() const;

  /// \return the list of to faces.
  GEOMALGOAPI_EXPORT const ListOfShape& toFaces() const;

protected:
  GeomAlgoAPI_MakeSweep(){};

protected:
  ListOfShape myFromFaces;
  ListOfShape myToFaces;
};

#endif
