// File:        PartSet_Tools.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_Tools_H
#define PartSet_Tools_H

#include "PartSet.h"

#include <ModuleBase_ViewerPrs.h>
#include <gp_Pnt.hxx>

#include <QPoint>
#include <QList>

#include <ModelAPI_CompositeFeature.h>

#include <boost/shared_ptr.hpp>

class Handle_V3d_View;
class ModuleBase_ViewerPrs;
class GeomDataAPI_Point2D;
class GeomAPI_Pln;
class GeomAPI_Pnt2d;
class GeomAPI_Pnt;
class GeomAPI_Edge;

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
  Handle(V3d_View) theView,
                          double& theX, double& theY);

  /// \brief Converts the 2D projected coodinates on the sketch plane to the 3D point.
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  /// \param theSketch the sketch feature
  /// \param thePoint the 3D point in the viewer
  static void convertTo3D(const double theX, const double theY, FeaturePtr theSketch,
                          gp_Pnt& thePoint);

  /// Returns a feature that is under the mouse point
  /// \param thePoint a screen point
  /// \param theView a 3D view
  /// \param theSketch the sketch feature
  /// \param theFeatures the list of selected presentations
  static FeaturePtr nearestFeature(QPoint thePoint, Handle_V3d_View theView, FeaturePtr theSketch,
                                   const QList<ModuleBase_ViewerPrs>& theFeatures);

  /// Returns pointer to the root document.
  static boost::shared_ptr<ModelAPI_Document> document();


  /// Returns a point attribute of the feature by the coordinates if it is
  /// \param theFeature the feature
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  static boost::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(FeaturePtr theFeature,
                                                                double theX, double theY);

  /// \brief Save the double to the feature. If the attribute is double, it is filled.
  /// \param theFeature the feature
  /// \param theValue the horizontal coordinate
  /// \param theAttribute the feature attribute
  static void setFeatureValue(FeaturePtr theFeature, double theX, const std::string& theAttribute);

  /// \brief Returns the feature double value if it is.
  /// \param theFeature the feature
  /// \param theAttribute the feature attribute
  /// \param isValid an output parameter whether the value is valid
  /// \returns the feature value
  static double featureValue(FeaturePtr theFeature, const std::string& theAttribute, bool& isValid);

  /// Find a feature in the attribute of the given feature. If the kind is not empty,
  /// the return feature should be this type. Otherwise it is null
  /// \param theFeature a source feature
  /// \param theAttribute a name of the requried attribute attribute
  /// \param theKind an output feature kind
  /// \return the feature
  static FeaturePtr feature(FeaturePtr theFeature, const std::string& theAttribute,
                            const std::string& theKind);

  /// Creates a constraint on two points
  /// \param thePoint1 the first point
  /// \param thePoint1 the second point
  static void createConstraint(CompositeFeaturePtr theSketch,
                               boost::shared_ptr<GeomDataAPI_Point2D> thePoint1,
                               boost::shared_ptr<GeomDataAPI_Point2D> thePoint2);

  /// Creates constrains of the current 
  /// \param theSketch a sketch feature
  /// \param theFeature a source feature
  /// \param theAttribute a name of the requried attribute attribute
  /// \param theClickedX the horizontal coordnate of the point
  /// \param theClickedY the vertical coordnate of the point
  static void setConstraints(CompositeFeaturePtr theSketch, FeaturePtr theFeature,
                             const std::string& theAttribute, double theClickedX,
                             double theClickedY);

  /// Create a sketch plane instance
  /// \param theSketch a sketch feature
  /// \return API object of geom plane
  static boost::shared_ptr<GeomAPI_Pln> sketchPlane(CompositeFeaturePtr theSketch);

  /// Create a point 3D on a basis of point 2D and sketch feature
  /// \param thePoint2D a point on a sketch
  /// \param theSketch a sketch feature
  /// \return API object of point 3D
  static boost::shared_ptr<GeomAPI_Pnt> point3D(boost::shared_ptr<GeomAPI_Pnt2d> thePoint2D,
                                                CompositeFeaturePtr theSketch);
  /// Check whether there is a constraint with the feature kind given
  /// \param theKind a feature kind
  /// \return the boolean value
  static bool isConstraintFeature(const std::string& theKind);

  /// Creates a line (arc or circle) by given edge
  /// Created line will have fixed constraint
  /// \param theEdge - an edge
  /// \return - result of created feature
  static ResultPtr createFixedObjectByEdge(const ModuleBase_ViewerPrs& thePrs, CompositeFeaturePtr theSketch);

  /// Checks whether the list of selected presentations contains the given one
  /// \param theSelected a list of presentations
  /// \param thePrs a presentation to be found
  /// \return - result of check, true if the list contains the prs
  static bool isContainPresentation(const QList<ModuleBase_ViewerPrs>& theSelected,
                                    const ModuleBase_ViewerPrs& thePrs);

  /// Returns Result object if the given skietch contains external edge equal to the given
  /// \param theSketch - the sketch feature
  /// \param theEdge - the edge
  /// \return result object with external edge if it is found
  static ResultPtr findExternalEdge(CompositeFeaturePtr theSketch, boost::shared_ptr<GeomAPI_Edge> theEdge);
};

#endif
