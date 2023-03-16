// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef GeomAlgoAPI_ShapeTools_H_
#define GeomAlgoAPI_ShapeTools_H_

#include "GeomAlgoAPI.h"

#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Pnt.h>
#include <map>
#include <set>
#ifdef WIN32
#include<array>
#endif

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
  /// \return the lenth of the edges of the current shape or 0.0 if it can be computed.
  GEOMALGOAPI_EXPORT static double length(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return the total volume of the solids of the current shape or 0.0 if it can be computed.
  GEOMALGOAPI_EXPORT static double volume(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return the total area of the faces of the current shape or 0.0 if it can be computed.
  GEOMALGOAPI_EXPORT static double area(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// indicate if two faces are continuous
  /// with an angular tolerance used for G1 continuity to compare the angle between the normals
  /// \param theFace1  the first face
  /// \param theFace2  the second face
  /// \param thePoint  the point for the normal
  /// \param theAngle  the angular tolerance
  /// \param theError  error
  GEOMALGOAPI_EXPORT static bool isContinuousFaces(const GeomShapePtr& theFace1,
                                                   const GeomShapePtr& theFace2,
                                                   const GeomPointPtr& thePoint,
                                                   const double & theAngle,
                                                   std::string& theError);

  /// \return the center of mass of the current face.
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
  GEOMALGOAPI_EXPORT static double minimalDistance(const GeomShapePtr& theShape1,
                                                   const GeomShapePtr& theShape2,
                                                   std::array<double, 3> & fromShape1To2);

  /// \brief Calculate maximal value of all possible distances between shapes.
  ///        The shapes can be two edges or two faces only.
  GEOMALGOAPI_EXPORT static double shapeProximity(const GeomShapePtr& theShape1,
                                                  const GeomShapePtr& theShape2);

  /// \brief Combines faces with common edges to shells, or solids to compsolids.
  /// \param[in] theCompound compound of shapes.
  /// \param[in] theType type of combine.
  /// \param[out] theResults resulting shapes.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> combineShapes(
    const std::shared_ptr<GeomAPI_Shape> theCompound,
    const GeomAPI_Shape::ShapeType theType, ListOfShape& theResults);

  /// \brief Groups shapes with shared topology to compounds.
  /// \param[in] theCompound compound of shapes.
  /// \return compound of compounds with shared topology.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape>
    groupSharedTopology(const std::shared_ptr<GeomAPI_Shape> theCompound);

  /// \brief Check group of shapes has shared sub-shapes of the given type
  /// \param[in] theShapes    list of shapes
  /// \param[in] theShapeType type of sub-shapes to check
  /// \return \c true if shared topology exists
  GEOMALGOAPI_EXPORT static bool hasSharedTopology(const ListOfShape& theShapes,
                                                   const GeomAPI_Shape::ShapeType theShapeType);

  /// \brief Calculates bounding box for theShapes
  /// \return list of eight points.
  /// \param[in] theShapes list of shapes.
  /// \param[in] theEnlarge enlarges bounding box size.
  GEOMALGOAPI_EXPORT static
    std::list<std::shared_ptr<GeomAPI_Pnt> > getBoundingBox(const ListOfShape& theShapes,
                                                            const double theEnlarge = 0.0);

  /// \brief Enlarges or reduces plane to fit bounding box.
  /// \return plane that fits to bounding box.
  /// \param[in] thePlane base plane.
  /// \param[in] thePoints bounding box points (should be eight).
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
  /// \return plane where all shapes lie or empty pointer if they not planar.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Pln> findPlane(const ListOfShape& theShapes);

  /// \brief Checks that vertex/edge is inside face or vertex inside wire.
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

  /// \return \c true if edges are tangent in the specified point
  GEOMALGOAPI_EXPORT static bool isTangent(const std::shared_ptr<GeomAPI_Edge> theEdge1,
                                           const std::shared_ptr<GeomAPI_Edge> theEdge2,
                                           const std::shared_ptr<GeomAPI_Vertex> theTgPoint);

  /// \return true if edge is parallel to face.
  GEOMALGOAPI_EXPORT static bool isParallel(const std::shared_ptr<GeomAPI_Edge> theEdge,
                                            const std::shared_ptr<GeomAPI_Face> theFace);

  /// Computes intersection point between the edge curve and a face surface (only one point, with
  /// preferences to point that belongs to edge and face boundaries.
  /// \returns null if there is no intersection
  GEOMALGOAPI_EXPORT static std::list<std::shared_ptr<GeomAPI_Vertex> > intersect(
    const std::shared_ptr<GeomAPI_Edge> theEdge, const std::shared_ptr<GeomAPI_Face> theFace);

  typedef std::map<std::shared_ptr<GeomAPI_Pnt>,
                   std::pair<std::list<std::shared_ptr<GeomDataAPI_Point2D> >,
                             std::list<std::shared_ptr<ModelAPI_Object> > > > PointToRefsMap;
  /// \brief Performs the split of the shape by points.
  /// \param[in] theBaseShape shape that should be divided.
  /// \param[in] thePoints container of points to split
  /// \param[out] theShapes container of shapes after split
  GEOMALGOAPI_EXPORT static void splitShape(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                      const PointToRefsMap& thePointsInfo,
                                      std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes);

  /// \brief Performs the split of the shape by points.
  /// \param[in] theBaseShape shape that should be divided.
  /// \param[in] thePoints container of points to split
  /// \param[out] theShapes container of shapes after split
  GEOMALGOAPI_EXPORT static void splitShape_p(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                      const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                      std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes);

  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> findShape(
                                    const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                    const std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes);

#ifdef FEATURE_MULTIROTATION_TWO_DIRECTIONS
  /// \brief Returns a direction from a shape and an axis.
  /// \param[in] theBaseShape shape whose center of mass serves as the starting point of the dir.
  /// \param[in] theAxis axis that serves as a direction for the dir
  /// \return dir that builds from center of mass of the base shape and the axis
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Dir> buildDirFromAxisAndShape(
                                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                    const std::shared_ptr<GeomAPI_Ax1> theAxis);
#endif

  /// \brief Re-approximate a wire to build a single edge
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Edge> wireToEdge(
      const std::shared_ptr<GeomAPI_Wire>& theWire);

  /// \brief Get non-composite sub-shapes of the given shape.
  /// \param[in] theShape shape that should be exploded
  /// \return list of sub-shapes (vertices, edges, faces, solids)
  GEOMALGOAPI_EXPORT static ListOfShape getLowLevelSubShapes(const GeomShapePtr& theShape);

  /// \brief Calculate prism sizes to ensure that it passes through all objects
  /// \param[in] theObjects objects to be joined/cutted by the prism
  /// \param[in] theBaseShapes bases of the prism
  /// \param[in] theDir direction of the prism
  /// \param[out] theToSize upper offset of the prism
  /// \param[out] theFromSize lower offset of the prism
  GEOMALGOAPI_EXPORT static void computeThroughAll(const ListOfShape& theObjects,
                                                   const ListOfShape& theBaseShapes,
                                                   const std::shared_ptr<GeomAPI_Dir> theDir,
                                                   double& theToSize, double& theFromSize);
};

#endif
