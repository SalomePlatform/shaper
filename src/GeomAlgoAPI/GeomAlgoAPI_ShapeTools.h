// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomAlgoAPI_ShapeTools_H_
#define GeomAlgoAPI_ShapeTools_H_

#include "GeomAlgoAPI.h"

#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>

#include <map>
#include <set>

class GeomAPI_Ax1;
class GeomAPI_Edge;
class GeomAPI_Dir;
class GeomAPI_Face;
class GeomAPI_PlanarEdges;
class GeomAPI_Pln;
class GeomAPI_Pnt;
class GeomAPI_Wire;
class GeomDataAPI_Point2D;
class ModelAPI_Object;

/// \class GeomAlgoAPI_ShapeTools
/// \ingroup DataAlgo
/// \brief Useful tools for working with shapes.
class GeomAlgoAPI_ShapeTools
{
public:
  /// \return the total volume of the solids of the current shape or 0.0 if it can be computed.
  GEOMALGOAPI_EXPORT static double volume(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return the total area of the faces of the current shape or 0.0 if it can be computed.
  GEOMALGOAPI_EXPORT static double area(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return the centre of mass of the current face.
  /// The coordinates returned for the center of mass
  /// are expressed in the absolute Cartesian coordinate system.
  /// (This function works only for surfaces).
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Pnt>
    centreOfMass(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \brief Obtain radius of cylindrical face.
  ///        Return negative value if the face is not a cylinder
  GEOMALGOAPI_EXPORT static double radius(const std::shared_ptr<GeomAPI_Face>& theCylinder);

  /// \brief Calculate minimal distance between shapes
  GEOMALGOAPI_EXPORT static double minimalDistance(const GeomShapePtr& theShape1,
                                                   const GeomShapePtr& theShape2);

  /// \brief Combines faces with common edges to shells, or solids to compsolids.
  /// \param[in] theCompound compound of shapes.
  /// \param[in] theType type of combine.
  /// \param[out] theCombinedShapes resulting shapes.
  /// \param[out] theFreeShapes shapes that does not have common subshapes.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> combineShapes(
    const std::shared_ptr<GeomAPI_Shape> theCompound,
    const GeomAPI_Shape::ShapeType theType,
    ListOfShape& theCombinedShapes,
    ListOfShape& theFreeShapes);

  /// \brief Groups shapes with shared topology to compounds.
  /// \param[in] theCompound compound of shapes.
  /// \return compound of compounds with shared topology.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape>
    groupSharedTopology(const std::shared_ptr<GeomAPI_Shape> theCompound);

  /// \brief Calculates bounding box for theShapes
  /// \return list of eight points.
  /// \param[in] theShapes list of shapes.
  /// \param[in] theEnlarge enlarges bounding box size.
  GEOMALGOAPI_EXPORT static
    std::list<std::shared_ptr<GeomAPI_Pnt> > getBoundingBox(const ListOfShape& theShapes,
                                                            const double theEnlarge = 0.0);

  /// \return infinite plane received from theFace plane.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape>
    faceToInfinitePlane(const std::shared_ptr<GeomAPI_Shape> theFace);

  /// \brief Enlarges or reduces plane to fit bounding box.
  /// \return plane that fits to bounding box.
  /// \param[in] thePlane base plane.
  /// \param[in] thePoints bounding box points (shoud be eight).
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Face> fitPlaneToBox(
    const std::shared_ptr<GeomAPI_Shape> thePlane,
    const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints);

  /// \brief Finds the start and end vertices of theShape. theShape can be of the following type:\n
  /// Vertex: theV1 and theV2 are the same and equal to theShape;\n
  /// Edge : theV1 is start and theV2 is end vertex;\n
  /// Wire : theV1 is start vertex of the first edge, theV2 is end vertex of the last edge. If wire
  /// contains no edges theV1 and theV2 are nullified.\n
  /// If none of the above theV1 and theV2 are nullified.
  GEOMALGOAPI_EXPORT static void findBounds(const std::shared_ptr<GeomAPI_Shape> theShape,
                                            std::shared_ptr<GeomAPI_Vertex>& theV1,
                                            std::shared_ptr<GeomAPI_Vertex>& theV2);

  /// \brief Creates faces with holes from wires.
  /// \param[in] theWires base wires.
  /// \param[out] theFaces resulting faces.
  GEOMALGOAPI_EXPORT static void makeFacesWithHoles(const std::shared_ptr<GeomAPI_Pnt> theOrigin,
                                                const std::shared_ptr<GeomAPI_Dir> theDirection,
                                                const ListOfShape& theWires,
                                                ListOfShape& theFaces);

  /// \brief Return a plane for list of shapes if they are all planar.
  /// \param[in] theShapes shapes to find plane.
  /// \return plane where all shapes lie or empty ptr if they not planar.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Pln> findPlane(const ListOfShape& theShapes);

  /// \brief Checks that vertex/edge is inside face or vertext inside wire.
  /// \param[in] theSubShape shape that should be inside.
  /// \param[in] theBaseShape base shape.
  /// \return true if edge inside the face.
  GEOMALGOAPI_EXPORT static bool isSubShapeInsideShape(
    const std::shared_ptr<GeomAPI_Shape> theSubShape,
    const std::shared_ptr<GeomAPI_Shape> theBaseShape);

  /// \return true if theShape is valid.
  GEOMALGOAPI_EXPORT static bool isShapeValid(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return outer wire for face. If theShape has different type returns empty pointer.
  GEOMALGOAPI_EXPORT static
    std::shared_ptr<GeomAPI_Shape> getFaceOuterWire(const std::shared_ptr<GeomAPI_Shape> theFace);

  /// \return true if edge is parallel to face.
  GEOMALGOAPI_EXPORT static bool isParallel(const std::shared_ptr<GeomAPI_Edge> theEdge,
                                            const std::shared_ptr<GeomAPI_Face> theFace);

  // Computes intersection point between the edge curve and a face surface (only one point, with
  // preferences to point that belongs to edge and face boundaries.
  /// \returns null if there is no intersection
  GEOMALGOAPI_EXPORT static std::list<std::shared_ptr<GeomAPI_Vertex> > intersect(
    const std::shared_ptr<GeomAPI_Edge> theEdge, const std::shared_ptr<GeomAPI_Face> theFace,
    const bool thePointsOutsideFace);

  typedef std::map<std::shared_ptr<GeomAPI_Pnt>,
                   std::pair<std::list<std::shared_ptr<GeomDataAPI_Point2D> >,
                             std::list<std::shared_ptr<ModelAPI_Object> > > > PointToRefsMap;
  /// \brief Performs the split of the shape by points.
  /// \param[in] theBaseShape shape that should be splitted.
  /// \param[in] thePoints container of points to split
  /// \param[out] theShapes container of shapes after split
  GEOMALGOAPI_EXPORT static void splitShape(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                      const PointToRefsMap& thePointsInfo,
                                      std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes);

  /// \brief Performs the split of the shape by points.
  /// \param[in] theBaseShape shape that should be splitted.
  /// \param[in] thePoints container of points to split
  /// \param[out] theShapes container of shapes after split
  GEOMALGOAPI_EXPORT static void splitShape_p(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                      const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                      std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes);

  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> findShape(
                                    const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                    const std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes);

  /// \brief Returns a dir from a shape and an axis.
  /// \param[in] theBaseShape shape whose center of mass serves as the starting point of the dir.
  /// \param[in] theAxis axis that serves as a direction for the dir
  /// \return dir that builds from center of mass of the base shape and the axis
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Dir> buildDirFromAxisAndShape(
                                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                    const std::shared_ptr<GeomAPI_Ax1> theAxis);

  /// \brief Reapproximate a wire to build a single edge
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Edge> wireToEdge(
      const std::shared_ptr<GeomAPI_Wire>& theWire);
};

#endif
