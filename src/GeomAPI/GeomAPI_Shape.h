// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef GeomAPI_Shape_H_
#define GeomAPI_Shape_H_

#include "GeomAPI_Dir.h"

#include <GeomAPI_Interface.h>
#include <memory>
#include <list>
#include <string>
class GeomAPI_Pnt;
class GeomAPI_Vertex;
class GeomAPI_Edge;
class GeomAPI_Wire;
class GeomAPI_Face;
class GeomAPI_Shell;
class GeomAPI_Solid;
class GeomAPI_Trsf;

/**\class GeomAPI_Shape
 * \ingroup DataModel
 * \brief Interface to the topological shape object
 */
class GeomAPI_Shape : public GeomAPI_Interface
{
public:
  /// Shape type enum
  enum ShapeType {
    COMPOUND, COMPSOLID, SOLID, SHELL,
    FACE, WIRE, EDGE, VERTEX,
    SHAPE
  };

  /// Shape orientation
  enum Orientation {
    FORWARD,
    REVERSED,
    INTERNAL,
    EXTERNAL
  };

 public:
  /// Creation of empty (null) shape
  GEOMAPI_EXPORT
  GeomAPI_Shape();

  /// \return a new Shape with the same Orientation and Location and a new TShape with the same
  ///         geometry and no sub-shapes.
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Shape> emptyCopied() const;

  /// Returns true if the underlied shape is null
  GEOMAPI_EXPORT
  bool isNull() const;

  /// Returns whether the shapes are equal
  GEOMAPI_EXPORT
  virtual bool isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns whether the shapes are same
  GEOMAPI_EXPORT
  virtual bool isSame(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns \c true if shapes have same underlying geometry
  GEOMAPI_EXPORT
  virtual bool isSameGeometry(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns whether the shape is a vertex
  GEOMAPI_EXPORT
  virtual bool isVertex() const;

  /// Returns whether the shape is an edge
  GEOMAPI_EXPORT
  virtual bool isEdge() const;

  /// Returns whether the shape is a wire
  GEOMAPI_EXPORT
  virtual bool isWire() const;

  /// Returns whether the shape is a face
  GEOMAPI_EXPORT
  virtual bool isFace() const;

  /// Returns whether the shape is a shell
  GEOMAPI_EXPORT
  virtual bool isShell() const;

  /// Returns whether the shape is a compound
  GEOMAPI_EXPORT
  virtual bool isCompound() const;

  /// Returns whether the shape is a compound of solids
  GEOMAPI_EXPORT
  virtual bool isCompoundOfSolids() const;

  /// Returns true, if the shape contains only solids, compsolids
  /// and compounds of solids and compsolids of any nesting depth
  GEOMAPI_EXPORT
  virtual bool isCollectionOfSolids() const;

  /// Returns whether the shape is a compound where all elements are topologically connected
  GEOMAPI_EXPORT
  virtual bool isConnectedTopology() const;

  /// Returns whether the shape is a solid
  GEOMAPI_EXPORT
  virtual bool isSolid() const;

  /// Returns whether the shape is a compsolid
  GEOMAPI_EXPORT
  virtual bool isCompSolid() const;

  /// Returns whether the shape is planar
  GEOMAPI_EXPORT
  virtual bool isPlanar() const;

  /// Returns vertex or empty shape
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Vertex> vertex() const;

  /// Returns edge or empty shape
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Edge> edge() const;

  /// Returns wire or empty shape
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Wire> wire() const;

  /// Returns face or empty shape
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Face> face() const;

  /// Returns shell or empty shape
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Shell> shell() const;

  /// Returns solid or empty shape
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Solid> solid() const;

  /// Returns list of sub-shapes of the given type
  /// \param theSubShapeType type of sub-shapes to search.
  /// \param theOnlyUnique set it to \c true to omit subsequent
  ///        inclusions of the same sub-shape. By default it is \c false.
  GEOMAPI_EXPORT
  std::list< std::shared_ptr<GeomAPI_Shape> > subShapes(const ShapeType theSubShapeType,
                                                        const bool theOnlyUnique = false) const;

  /// Returns the shape type
  GEOMAPI_EXPORT
  virtual ShapeType shapeType() const;

  /// Returns the type enumeration by the string-type
  GEOMAPI_EXPORT static ShapeType shapeTypeByStr(std::string theType);

  /// \return the shape type as string.
  GEOMAPI_EXPORT
  virtual std::string shapeTypeStr() const;

  /// \return the shape orientation.
  GEOMAPI_EXPORT virtual Orientation orientation() const;

  /// Sets the shape orientation.
  GEOMAPI_EXPORT virtual void setOrientation(const Orientation theOrientation);

  /// Reverse shape
  GEOMAPI_EXPORT virtual void reverse();

  /// \return true if passed shape is a sub-shape of this shape.
  /// \param theShape shape to search.
  /// \param theCheckOrientation if false, returns true even if orientation of shape differs
  GEOMAPI_EXPORT virtual bool isSubShape(const std::shared_ptr<GeomAPI_Shape> theShape,
    const bool theCheckOrientation = true) const;

  /// Computes boundary dimensions of the shape
  /// Returns False if it is not possible
  GEOMAPI_EXPORT
  bool computeSize(double& theXmin, double& theYmin, double& theZmin,
                   double& theXmax, double& theYmax, double& theZmax) const;

  /// Return middle point on the shape
  GEOMAPI_EXPORT
  virtual std::shared_ptr<GeomAPI_Pnt> middlePoint() const;

  /// Returns the shape as BRep stream
  GEOMAPI_EXPORT
  std::string getShapeStream(const bool theWithTriangulation = true) const;

  /// Returns intersection of shapes
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Shape> intersect(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns true if min distance between shapes < tolerance.
  GEOMAPI_EXPORT
  bool isIntersect(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Translates the shape along the direction for the given offset
  GEOMAPI_EXPORT
  void translate(const std::shared_ptr<GeomAPI_Dir> theDir, const double theOffset);

  /// Moves the shape with the given transformation matrix.
  GEOMAPI_EXPORT
    void move(const std::shared_ptr<GeomAPI_Trsf> theTransformation);

  /// Returns type of shapes in the compound.
  // If shapes are of different type then it will return SHAPE type
  GEOMAPI_EXPORT ShapeType typeOfCompoundShapes() const;

  /// Returns true if shape have self-intersections.
  /// \param[in] theLevelOfCheck defines which interferences will be checked:<br>
  /// 0 - only V/V;<br>
  /// 1 - V/V and V/E;<br>
  /// 2 - V/V, V/E and E/E;<br>
  /// 3 - V/V, V/E, E/E and V/F;<br>
  /// 4 - V/V, V/E, E/E, V/F and E/F;<br>
  /// 5 - V/V, V/E, E/E, V/F, E/F and F/F;<br>
  /// 6 - V/V, V/E, E/E, V/F, E/F, F/F and V/S;<br>
  /// 7 - V/V, V/E, E/E, V/F, E/F, F/F, V/S and E/S;<br>
  /// 8 - V/V, V/E, E/E, V/F, E/F, F/F, V/S, E/S and F/S;<br>
  /// 9 - V/V, V/E, E/E, V/F, E/F, F/F, V/S, E/S, F/S and S/S - all interferences (Default value)
  GEOMAPI_EXPORT bool isSelfIntersected(const int theLevelOfCheck = 9) const;

public:
  /// \brief Compare addresses of shapes
  class Comparator
  {
  public:
    /// Return \c true if the address of the first shape is less than the address of the second
    GEOMAPI_EXPORT
    bool operator ()(const std::shared_ptr<GeomAPI_Shape>& theShape1,
                     const std::shared_ptr<GeomAPI_Shape>& theShape2) const;
  };

  /// \brief Compare addresses of shapes with respect to orientation of shapes.
  ///        Same TShapes with opposite orientations will be treated as different.
  class ComparatorWithOri
  {
  public:
    /// Return \c true if the address of the first shape is less than the address of the second
    GEOMAPI_EXPORT
      bool operator ()(const std::shared_ptr<GeomAPI_Shape>& theShape1,
                       const std::shared_ptr<GeomAPI_Shape>& theShape2) const;
  };

  /// \brief Hash code for the shape
  class Hash
  {
  public:
    /// Return Hash value according to the address of the shape
    GEOMAPI_EXPORT
    int operator ()(const std::shared_ptr<GeomAPI_Shape>& theShape) const;
  };

  /// \brief Compare addresses of shapes
  class Equal
  {
  public:
    /// Return \c true if the address of the shapes are equal
    GEOMAPI_EXPORT
    bool operator ()(const std::shared_ptr<GeomAPI_Shape>& theShape1,
                     const std::shared_ptr<GeomAPI_Shape>& theShape2) const;
  };
};

//! Pointer on list of shapes
typedef std::list<std::shared_ptr<GeomAPI_Shape> > ListOfShape;

//! Pointer on attribute object
typedef std::shared_ptr<GeomAPI_Shape> GeomShapePtr;

#endif
