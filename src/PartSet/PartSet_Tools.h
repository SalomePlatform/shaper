// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Tools.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_Tools_H
#define PartSet_Tools_H

#include "PartSet.h"

#include <gp_Pnt.hxx>

#include <QPoint>
#include <QList>

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>

#include <Events_Message.h>

#include <TopoDS_Shape.hxx>

#include <memory>

class V3d_View;
class ModuleBase_ViewerPrs;
class ModuleBase_IWorkshop;
class GeomDataAPI_Point2D;
class GeomAPI_Shape;
class GeomAPI_Pln;
class GeomAPI_Pnt2d;
class GeomAPI_Pnt;
class GeomAPI_Edge;
class GeomAPI_Vertex;
class ModelAPI_Result;

/*!
 * \class PartSet_Tools
 * \ingroup Modules
 * \brief The operation for the sketch feature creation
 */
class PARTSET_EXPORT PartSet_Tools
{
public:
  /// \enum ConstraintVisibleState types of constraints which visible state can be
  /// changed in the sketch widget
  enum ConstraintVisibleState
  {
    Geometrical = 0, // all constrains excepting dimensional
    Dimensional,     // lenght, distance, radius and angle constraints
    Expressions,     // parameter text should be shown in dimensional constraint
    Any              // both, geometrical and dimensional, types of constraints
  };

 public:
  /// Returns default value of AIS presentation
  /// \return integer value
  static int getAISDefaultWidth();

  /// Converts the 2D screen point to the 3D point on the view according to the point of view
  /// \param thePoint a screen point
  /// \param theView a 3D view
  static gp_Pnt convertClickToPoint(QPoint thePoint, Handle(V3d_View) theView);

  /// \brief Converts the 3D point to the projected coodinates on the sketch plane.
  /// \param thePoint the 3D point in the viewer
  /// \param theSketch the sketch feature
  /// \param theView a view 3d object
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  static void convertTo2D(const gp_Pnt& thePoint, FeaturePtr theSketch,
                          Handle(V3d_View) theView,
                          double& theX, double& theY);

  /// \brief Converts the 3D point to the projected coodinates on the sketch plane.
  /// \param theSketch the sketch feature
  /// \param thePnt the 3D point in the viewer
  /// \returns the converted point object
  static std::shared_ptr<GeomAPI_Pnt2d> convertTo2D(FeaturePtr theSketch,
                                                    const std::shared_ptr<GeomAPI_Pnt>& thePnt);

  /// \brief Converts the 2D projected coodinates on the sketch plane to the 3D point.
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  /// \param theSketch the sketch feature
  static std::shared_ptr<GeomAPI_Pnt> convertTo3D(const double theX, const double theY,
                                                  FeaturePtr theSketch);

  /// Returns pointer to the root document.
  static std::shared_ptr<ModelAPI_Document> document();

  /// \brief Save the double to the feature. If the attribute is double, it is filled.
  /// \param theFeature the feature
  /// \param theX the horizontal coordinate
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
  /// \param theSketch a sketch feature
  /// \param thePoint1 the first point
  /// \param thePoint2 the second point
  static void createConstraint(CompositeFeaturePtr theSketch,
                               std::shared_ptr<GeomDataAPI_Point2D> thePoint1,
                               std::shared_ptr<GeomDataAPI_Point2D> thePoint2);

  /// Finds in the feature's sub-features first Point2D attribute with the given point coordinates
  /// \param theFeature a feature with sub-feature attributes
  /// \param thePoint a point to provided searched coordinates
  /// \return found point or null
  static std::shared_ptr<GeomDataAPI_Point2D> findFirstEqualPointInArgumentFeatures(
                  const FeaturePtr& theFeature, const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Finds in the feature first Point2D attribute with the given point coordinates
  /// \param theFeature a feature with point attributes
  /// \param thePoint a point to provided searched coordinates
  /// \return found point or null
  static std::shared_ptr<GeomDataAPI_Point2D> findFirstEqualPoint(const FeaturePtr& theFeature,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  static std::shared_ptr<GeomDataAPI_Point2D> findFirstEqualPointInSketch(
                                       const CompositeFeaturePtr& theSketch,
                                       const std::shared_ptr<GeomDataAPI_Point2D>& theFeaturePoint,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Create a sketch plane instance
  /// \param theSketch a sketch feature
  /// \return API object of geom plane
  static std::shared_ptr<GeomAPI_Pln> sketchPlane(CompositeFeaturePtr theSketch);

  /// Create a point 3D on a basis of point 2D and sketch feature
  /// \param thePoint2D a point on a sketch
  /// \param theSketch a sketch feature
  /// \return API object of point 3D
  static std::shared_ptr<GeomAPI_Pnt> point3D(std::shared_ptr<GeomAPI_Pnt2d> thePoint2D,
                                                CompositeFeaturePtr theSketch);

  /// Finds a line (arc or circle) by given edge
  /// \param theShape an edge
  /// \param theObject a selected result object
  /// \param theSketch a sketch feature
  /// \return result of found feature or NULL
  static std::shared_ptr<ModelAPI_Result> findFixedObjectByExternal(
                                             const TopoDS_Shape& theShape,
                                             const ObjectPtr& theObject,
                                             CompositeFeaturePtr theSketch);

  /// Creates a line (arc or circle) by given edge
  /// Created line will have fixed constraint
  /// \param theShape an edge
  /// \param theObject a selected result object
  /// \param theSketch a sketch feature
  /// \param theTemporary the created external object is temporary, execute is not performed for it
  /// \return result of created feature
  static std::shared_ptr<ModelAPI_Result> createFixedObjectByExternal(
                                               const TopoDS_Shape& theShape,
                                               const ObjectPtr& theObject,
                                               CompositeFeaturePtr theSketch,
                                               const bool theTemporary = false);

    /// Checks whether the list of selected presentations contains the given one
  /// \param theSelected a list of presentations
  /// \param thePrs a presentation to be found
  /// \return - result of check, true if the list contains the prs
  static bool isContainPresentation(const QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theSelected,
                                    const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Returns Result object if the given skietch contains external edge equal to the given
  /// \param theSketch - the sketch feature
  /// \param theEdge - the edge
  /// \return result object with external edge if it is found
  static std::shared_ptr<ModelAPI_Result> findExternalEdge(CompositeFeaturePtr theSketch,
                                                           std::shared_ptr<GeomAPI_Edge> theEdge);

  /// Returns Result object if the given sketch contains external vertex equal to the given
  /// \param theSketch - the sketch feature
  /// \param theVert - the vertex
  /// \return result object with external vertex if it is found
  static std::shared_ptr<ModelAPI_Result> findExternalVertex(CompositeFeaturePtr theSketch,
                                                        std::shared_ptr<GeomAPI_Vertex> theVert);

  /// Returns whether the selected presentation has a shape with the vertex type
  /// \param thePrs a selected presentation
  /// \param theSketch the sketch feature
  /// \param theView a 3D view
  /// \param theX the output horizontal coordinate of the point
  /// \param theY the output vertical coordinate of the point
  static bool hasVertexShape(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
                             FeaturePtr theSketch,
                             Handle(V3d_View) theView, double& theX, double& theY);


  /**
  * Find attribute of object which corresponds to the given shape
  * \param theObj - an object
  * \param theShape - a Shape
  * \param theSketch - a Sketch to get a plane of converting to 2d
  */
  static AttributePtr findAttributeBy2dPoint(ObjectPtr theObj, const TopoDS_Shape theShape,
                                             FeaturePtr theSketch);

  /**
  * Finds an attribute value in attribute reference attribute value
  * \param theAttribute - an attribure reference filled with an attribute
  * \param theWorkshop a reference to workshop
  * \return a geometry shape
  */
  static std::shared_ptr<GeomAPI_Shape> findShapeBy2DPoint(const AttributePtr& theAttribute,
                                         ModuleBase_IWorkshop* theWorkshop);

  /**
  * Returns point of coincidence feature
  * \param theFeature the coincidence feature
  * \param theAttribute the attribute name
  */
  static std::shared_ptr<GeomAPI_Pnt2d> getPoint(std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                 const std::string& theAttribute);

  /**
  * Gets all references to the feature, take coincidence constraint features, get point 2d attributes
  * and compare the point value to be equal with the given. Returns the first feature, which has
  * equal points.
  * \param theFeature the coincidence feature
  * \param thePoint a 2d point
  * \return the coincidence feature or null
  */
  static FeaturePtr findFirstCoincidence(const FeaturePtr& theFeature,
                                         std::shared_ptr<GeomAPI_Pnt2d> thePoint);

  /**
  * Returns list of features connected in a councedence feature point
  * \param theStartCoin the coincidence feature
  * \param theList a list which collects lines features
  * \param theCoincidencies a list of coincidence features
  * \param theAttr the attribute name
  */
  static void findCoincidences(FeaturePtr theStartCoin, QList<FeaturePtr>& theList,
                               QList<FeaturePtr>& theCoincidencies,
                               std::string theAttr);

  /**
  * Returns point of a coincedence
  * \param theStartCoin the coincedence feature
  */
  static std::shared_ptr<GeomAPI_Pnt2d> getCoincedencePoint(FeaturePtr theStartCoin);

  /// Sends redisplay event for all sub-features of the composite. Flush it.
  static void sendSubFeaturesEvent(const CompositeFeaturePtr& theComposite,
                                   const Events_ID theId);

  /**
   * Returns true if the object is a sketch entity, where auxiliary attribute has true vlaue
   * \param theObject a result or feature
   * \return boolean result
   */
  static bool isAuxiliarySketchEntity(const ObjectPtr& theObject);
};

#endif
