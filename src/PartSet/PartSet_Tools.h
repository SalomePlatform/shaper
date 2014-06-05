// File:        PartSet_Tools.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_Tools_H
#define PartSet_Tools_H

#include "PartSet.h"

#include <gp_Pnt.hxx>

#include <QPoint>

#include <ModelAPI_Feature.h>

#include <boost/shared_ptr.hpp>

#include <list>

class Handle_V3d_View;
class XGUI_ViewerPrs;
class GeomDataAPI_Point2D;

/*!
 \class PartSet_Tools
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_Tools
{
public:
  /// Converts the 2D screen point to the 3D point on the view according to the point of view
  /// \param thePoint a screen point
  /// \param theView a 3D view
  static gp_Pnt convertClickToPoint(QPoint thePoint, Handle_V3d_View theView);

  /// \brief Converts the 3D point to the projected coodinates on the sketch plane.
  /// \param thePoint the 3D point in the viewer
  /// \param theSketch the sketch feature
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  static void convertTo2D(const gp_Pnt& thePoint, FeaturePtr theSketch,
                          Handle(V3d_View) theView, double& theX, double& theY);

  /// \brief Converts the 2D projected coodinates on the sketch plane to the 3D point.
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  /// \param theSketch the sketch feature
  /// \param thePoint the 3D point in the viewer
  static void convertTo3D(const double theX, const double theY, FeaturePtr theSketch,
                          gp_Pnt& thePoint);

  /// Returns the point of intersection of the two lines, the first is (v0, v1), the second is (v2, v3),
  /// where vi - {xi,yi}. If the v0 is on the second line, the result is a projection of the v1 to this line
  /// \param theX0 the horizontal coordinate of 0 point
  /// \param theY0 the vertical coordinate of 0 point
  /// \param theX1 the horizontal coordinate of 1 point
  /// \param theY1 the vertical coordinate of 1 point
  /// \param theX2 the horizontal coordinate of 2 point
  /// \param theY2 the vertical coordinate of 2 point
  /// \param theX3 the horizontal coordinate of 3 point
  /// \param theY3 the vertical coordinate of 3 point
  /// \param theX the output horizontal coordinate of the intersection point
  /// \param theY the outpup vertical coordinate of the intersection point
  static void intersectLines(double theX0, double theY0, double theX1, double theY1,
                             double theX2, double theY2, double theX3, double theY3,
                             double& theX, double& theY);

  /// Returns the coordinates of projection of the point to the line
  /// \param thePointX the projected point horizontal coordinate
  /// \param thePointY the projected point vertictal coordinate
  /// \param theX1 the horizontal coordinate of the first line point
  /// \param theY1 the vertical coordinate of the first line point
  /// \param theX2 the horizontal coordinate of the second line point
  /// \param theY2 the vertical coordinate of the second line point
  static void projectPointOnLine(double theX1, double theY1, double theX2, double theY2,
                                 double thePointX, double thePointY, double& theX, double& theY);

  /// Returns a feature that is under the mouse point
  /// \param thePoint a screen point
  /// \param theView a 3D view
  /// \param theSketch the sketch feature
  /// \param theFeatures the list of selected presentations
  static FeaturePtr nearestFeature(QPoint thePoint, Handle_V3d_View theView, FeaturePtr theSketch,
                                   const std::list<XGUI_ViewerPrs>& theFeatures);

  /// Returns pointer to the root document.
  static boost::shared_ptr<ModelAPI_Document> document();

  /// \brief Save the point to the feature. If the attribute is 2D geometry point, it is filled.
  /// \param theFeature the feature
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  /// \param theAttribute the feature attribute
  static void setFeaturePoint(FeaturePtr, double theX, double theY, const std::string& theAttribute);

    /// Creates a constraint on two points
  /// \param thePoint1 the first point
  /// \param thePoint1 the second point
  static void createConstraint(FeaturePtr theSketch,
                               boost::shared_ptr<GeomDataAPI_Point2D> thePoint1,
                               boost::shared_ptr<GeomDataAPI_Point2D> thePoint2);

  /// \brief Get the line point 2d coordinates.
  /// \param theFeature the line feature
  /// \param theAttribute the start or end attribute of the line
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  static void getLinePoint(FeaturePtr theFeature, const std::string& theAttribute,
                           double& theX, double& theY);
  /// Find a point in the line with given coordinates
  /// \param theFeature the line feature
  /// \param theX the horizontal point coordinate
  /// \param theY the vertical point coordinate
  static boost::shared_ptr<GeomDataAPI_Point2D> findPoint(FeaturePtr theFeature, double theX,
                                                          double theY);

private:
  /// Return the distance between the feature and the point
  /// \param theFeature feature object
  /// \param theX the horizontal coordinate of the point
  /// \param theX the vertical coordinate of the point
  static double distanceToPoint(FeaturePtr theFeature, double theX, double theY);
};

#endif
