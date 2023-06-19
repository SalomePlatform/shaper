// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef GeomAPI_AISObject_H_
#define GeomAPI_AISObject_H_

#include <GeomAPI_Interface.h>

#include <memory>

class GeomAPI_Circ;
class GeomAPI_Lin;
class GeomAPI_Pln;
class GeomAPI_Pnt;
class GeomAPI_Shape;

/** \class GeomAPI_AISObject
 *  \ingroup DataModel
 *  \brief Interface for AIS_InteractiveObject
 */

class GeomAPI_AISObject : public GeomAPI_Interface
{
 public:
  /// \brief Creation of empty AIS object
  GEOMAPI_EXPORT
  GeomAPI_AISObject();

  GEOMAPI_EXPORT
  ~GeomAPI_AISObject();

  /// \brief Creates AIS_Shape object using specified shape
  GEOMAPI_EXPORT
  void createShape(std::shared_ptr<GeomAPI_Shape> theShape);

  /// Returns shape used for the presentation creation (can be NULL)
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Shape> getShape() const;

  /** \brief Creates PrsDim_LengthDimension object
   *  \param[in] theStartPoint  first point for dimension
   *  \param[in] theEndPoint    second point for dimension
   *  \param[in] theFlyoutPoint the flyout of dimension relative to the measured line
   *  \param[in] thePlane       the plane which contains all points above
   *  \param[in] theDistance    value of the distance to be shown
   */
  GEOMAPI_EXPORT
  void createDistance(std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                      std::shared_ptr<GeomAPI_Pnt> theEndPoint,
                      std::shared_ptr<GeomAPI_Pnt> theFlyoutPoint,
                      std::shared_ptr<GeomAPI_Pln> thePlane, double theDistance);

  /**
   * Returns validity of the AIS distance. It is invalid if set measured geometry is not valid,
   * e.g. the distance points are equal.
   * \return a boolean result
   */
  GEOMAPI_EXPORT
  bool isEmptyDistanceGeometry();

  /** \brief Creates PrsDim_RadiusDimension object
   *  \param[in] theCircle      the radius is created for this circle
   *  \param[in] theFlyoutPoint the flyout of dimension
   *  \param[in] theRadius      value of the radius to be shown
   */
  GEOMAPI_EXPORT
  void createRadius(std::shared_ptr<GeomAPI_Circ> theCircle,
                    std::shared_ptr<GeomAPI_Pnt> theFlyoutPoint, double theRadius);

  /** \brief Creates PrsDim_ParallelRelation object for two lines
   *  \param[in] theLine1       first parallel line
   *  \param[in] theLine2       second parallel line
   *  \param[in] theFlyoutPoint the flyout point for relation
   *  \param[in] thePlane       the plane which contains the lines
   */
  GEOMAPI_EXPORT
  void createParallel(std::shared_ptr<GeomAPI_Shape> theLine1,
                      std::shared_ptr<GeomAPI_Shape> theLine2,
                      std::shared_ptr<GeomAPI_Pnt> theFlyoutPoint,
                      std::shared_ptr<GeomAPI_Pln> thePlane);

  /** \brief Creates PrsDim_PerpendicularRelation object for two lines
   *  \param[in] theLine1       first parallel line
   *  \param[in] theLine2       second parallel line
   *  \param[in] thePlane       the plane which contains the lines
   */
  GEOMAPI_EXPORT
  void createPerpendicular(std::shared_ptr<GeomAPI_Shape> theLine1,
                           std::shared_ptr<GeomAPI_Shape> theLine2,
                           std::shared_ptr<GeomAPI_Pln> thePlane);

  /** \brief Creates PrsDim_FixedRelation object for an object
   *  \param[in] theShape       the object
   *  \param[in] thePlane       the plane which contains the lines
   */
  GEOMAPI_EXPORT
  void createFixed(std::shared_ptr<GeomAPI_Shape> theShape,
                   std::shared_ptr<GeomAPI_Pln> thePlane);

  /** \brief Assigns the color for the shape
   *  \param[in] theColor index of the color
   */
  GEOMAPI_EXPORT
  void setColor(const int& theColor);

  /** \brief Assigns the color for the shape
   *  \param[in] theR value of the red component
   *  \param[in] theG value of the green component
   *  \param[in] theB value of the blue component
   *  \returns true if the presentation color is changed
   */
  GEOMAPI_EXPORT
  bool setColor(int theR, int theG, int theB);

  /** \brief Returns the color for the shape
   *  \param[in] theR value of the red component
   *  \param[in] theG value of the green component
   *  \param[in] theB value of the blue component
   */
  GEOMAPI_EXPORT
  void getColor(int& theR, int& theG, int& theB);

  /** \brief Assigns the deflection to the shape
   *  \param[in] theDeflection value of deflection
   */
  GEOMAPI_EXPORT
  bool setDeflection(const double theDeflection);

  /** \brief Returns deflection for the shape
   *  \return double value
   */
  GEOMAPI_EXPORT
  double getDeflection() const;

  /** \brief Assigns the transparency to the shape
   *  \param[in] theTransparency value of transparency
   */
  GEOMAPI_EXPORT
  bool setTransparency(const double theTransparency);

  /** \brief Returns deflection for the shape
   *  \return double value
   */
  GEOMAPI_EXPORT
  double getTransparency() const;

  /// \return Current width of the lines of shape
  GEOMAPI_EXPORT
  double width();

  /// \brief Assigns the width of the lines of shape
  GEOMAPI_EXPORT
  bool setWidth(const double& theWidth);

  /// \brief Checks if the object is empty
  GEOMAPI_EXPORT
  bool empty() const;

  /// Return shape type according to TopAbs_ShapeEnum if the AIS is AIS_Shape
  /// Otherwise returns -1
  GEOMAPI_EXPORT
  int getShapeType() const;

  /// Sets marker type for vertex.
  /// The type has to be defined according to Acpect_TypeOfMarker
  GEOMAPI_EXPORT
  void setPointMarker(int theType, double theScale);

  /// Set line type of edges
  /// Has to be defined according to Aspect_TypeOfLine
  /// \returns true if the object value differs from the current
  GEOMAPI_EXPORT
  bool setLineStyle(int theStyle);

  /// Set transparency of the presentation (theVal = 0 ... 1)
  /// \returns true if the object value differs from the current
  GEOMAPI_EXPORT
  bool setTransparensy(double theVal);
};

//! Pointer on attribute object
typedef std::shared_ptr<GeomAPI_AISObject> AISObjectPtr;

#endif

