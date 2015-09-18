// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_SketchBuilder.h
// Created:     02 Jun 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAlgoAPI_SketchBuilder_H_
#define GeomAlgoAPI_SketchBuilder_H_

#include <GeomAlgoAPI.h>

#include <memory>
#include <list>

#include <GeomAPI_Dir.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>

/** \class GeomAlgoAPI_SketchBuilder
 *  \ingroup DataAlgo
 *  \brief Creates planar faces based on the list of Sketch features
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_SketchBuilder
{
 public:
  /** \brief Creates list of faces and unclosed wires on basis of the features of the sketch
   *  \param[in]  theOrigin      origin point of the sketch
   *  \param[in]  theDirX        x-direction of the sketch
   *  \param[in]  theNorm        normal of the sketch
   *  \param[in]  theFeatures    initial features of the sketch
   *  \param[out] theResultFaces faces based on closed wires
   *  \param[out] theResultWires unclosed wires of the sketch
   *
   *  The algorithm searches all loops of edges surrounding lesser squares.
   *  It finds the vertex with minimal coordinates along X axis (theDirX) and then 
   *  goes through the edges passing the surrounding area on the left.
   */
  static void createFaces(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                          const std::shared_ptr<GeomAPI_Dir>& theDirX,
                          const std::shared_ptr<GeomAPI_Dir>& theNorm,
                          const std::list<std::shared_ptr<GeomAPI_Shape> >& theFeatures,
                          std::list<std::shared_ptr<GeomAPI_Shape> >& theResultFaces,
                          std::list<std::shared_ptr<GeomAPI_Shape> >& theResultWires);

  /** \brief Creates list of faces and unclosed wires on basis of the features of the sketch
   *  \param[in]  theOrigin      origin point of the sketch
   *  \param[in]  theDirX        x-direction of the sketch
   *  \param[in]  theNorm        normal of the sketch
   *  \param[in]  theWire        a wire which contains all edges
   *  \param[out] theResultFaces faces based on closed wires
   *
   *  The algorithm searches all loops of edges surrounding lesser squares.
   *  It finds the vertex with minimal coordinates along X axis (theDirX) and then
   *  goes through the edges passing the surrounding area on the left.
   */
  static void createFaces(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                          const std::shared_ptr<GeomAPI_Dir>& theDirX,
                          const std::shared_ptr<GeomAPI_Dir>& theNorm,
                          const std::shared_ptr<GeomAPI_Shape>& theWire,
                          std::list<std::shared_ptr<GeomAPI_Shape> >& theResultFaces);

  /** \brief Searches intersections between the faces in the list 
   *         and make holes in the faces to avoid intersections
   *  \param[in,out] theFaces list of faces to proccess
   */
  static void fixIntersections(std::list<std::shared_ptr<GeomAPI_Shape> >& theFaces);
};

#endif
