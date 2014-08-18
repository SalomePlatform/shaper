// File:        GeomAPI_AISObject.hxx
// Created:     25 Jun 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_AISObject_H_
#define GeomAPI_AISObject_H_

#include <GeomAPI_Interface.h>

#include <boost/shared_ptr.hpp>

class GeomAPI_Circ;
class GeomAPI_Lin;
class GeomAPI_Pln;
class GeomAPI_Pnt;
class GeomAPI_Shape;

struct GEOMAPI_EXPORT Colors
{
  static int COLOR_BROWN;
  static int COLOR_RED;
  static int COLOR_GREEN;
  static int COLOR_BLUE;
};

/** \class GeomAPI_AISObject
 *  \ingroup DataModel
 *  \brief Interface for AIS_InteractiveObject
 */

class GEOMAPI_EXPORT GeomAPI_AISObject : public GeomAPI_Interface
{
public:
  /// \brief Creation of empty AIS object
  GeomAPI_AISObject();

  /// \brief Creates AIS_Shape object using specified shape
  void createShape(boost::shared_ptr<GeomAPI_Shape> theShape);

  /** \brief Creates AIS_LengthDimension object
   *  \param[in] theStartPoint  first point for dimension
   *  \param[in] theEndPoint    second point for dimension
   *  \param[in] theFlyoutPoint the flyout of dimension relative to the measured line
   *  \param[in] thePlane       the plane which contains all points above
   *  \param[in] theDistance    value of the distance to be shown
   */
  void createDistance(boost::shared_ptr<GeomAPI_Pnt> theStartPoint,
                      boost::shared_ptr<GeomAPI_Pnt> theEndPoint,
                      boost::shared_ptr<GeomAPI_Pnt> theFlyoutPoint,
                      boost::shared_ptr<GeomAPI_Pln> thePlane,
                      double                         theDistance);

  /** \brief Creates AIS_RadiusDimension object
   *  \param[in] theCircle      the radius is created for this circle
   *  \param[in] theFlyoutPoint the flyout of dimension
   *  \param[in] theRadius      value of the radius to be shown
   */
  void createRadius(boost::shared_ptr<GeomAPI_Circ> theCircle,
                    boost::shared_ptr<GeomAPI_Pnt>  theFlyoutPoint,
                    double                          theRadius);

  /** \brief Creates AIS_ParallelRelation object for two lines
   *  \param[in] theLine1       first parallel line
   *  \param[in] theLine2       second parallel line
   *  \param[in] theFlyoutPoint the flyout point for relation
   *  \param[in] thePlane       the plane which contains the lines
   */
  void createParallel(boost::shared_ptr<GeomAPI_Shape> theLine1,
                      boost::shared_ptr<GeomAPI_Shape> theLine2,
                      boost::shared_ptr<GeomAPI_Pnt>   theFlyoutPoint,
                      boost::shared_ptr<GeomAPI_Pln>   thePlane);

  /** \brief Creates AIS_PerpendicularRelation object for two lines
   *  \param[in] theLine1       first parallel line
   *  \param[in] theLine2       second parallel line
   *  \param[in] thePlane       the plane which contains the lines
   */
  void createPerpendicular(boost::shared_ptr<GeomAPI_Shape> theLine1,
                           boost::shared_ptr<GeomAPI_Shape> theLine2,
                           boost::shared_ptr<GeomAPI_Pln>   thePlane);

  /** \brief Assigns the color for the shape
   *  \param[in] theColor index of the color
   */
  void setColor(const int& theColor);

  /** \brief Assigns the color for the shape
   *  \param[in] theR value of the red component
   *  \param[in] theG value of the green component
   *  \param[in] theB value of the blue component
   */
  void setColor(int theR, int theG, int theB);

  /// \brief Assigns the width of the lines of shape
  void setWidth(const double& theWidth);

  /// \brief Checks if the object is empty
  bool empty() const;
};

#endif

