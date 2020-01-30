// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef PartSet_Tools_H
#define PartSet_Tools_H

#include "PartSet.h"

#include <gp_Pnt.hxx>

#include <QPoint>
#include <QList>
#include <QCursor>

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeSelection.h>

#include <Events_Message.h>

#include <TopoDS_Shape.hxx>

#include <memory>

class V3d_View;
class ModuleBase_IViewWindow;
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

class QMouseEvent;

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

  /// Create a sketch plane instance
  /// \param theSketch a sketch feature
  /// \return API object of geom plane
  static std::shared_ptr<GeomAPI_Pln> sketchPlane(CompositeFeaturePtr theSketch);

  /// Create a sketch plane instance
  /// \param theSketch a sketch feature
  /// \return API object of geom plane
  static void nullifySketchPlane(CompositeFeaturePtr theSketch);

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
  /// \param theCreatedFeature a new projection feature
  /// \return result of created feature
  static std::shared_ptr<ModelAPI_Result> createFixedObjectByExternal(
                                               const std::shared_ptr<GeomAPI_Shape>& theShape,
                                               const ObjectPtr& theObject,
                                               CompositeFeaturePtr theSketch,
                                               const bool theTemporary,
                                               FeaturePtr& theCreatedFeature);

    /// Checks whether the list of selected presentations contains the given one
  /// \param theSelected a list of presentations
  /// \param thePrs a presentation to be found
  /// \return - result of check, true if the list contains the prs
  static bool isContainPresentation(const QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theSelected,
                                    const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /**
  * Find attribute of object which corresponds to the given shape
  * \param theObj - an object
  * \param theShape - a Shape
  * \param theSketch - a Sketch to get a plane of converting to 2d
  * \return Found attribute and index of point if the attribute is an array
  */
  static std::pair<AttributePtr, int> findAttributeBy2dPoint(ObjectPtr theObj,
                                                             const TopoDS_Shape theShape,
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
  * Convertes parameters into a geom point
  * \param theEvent a Qt event to find mouse position
  * \param theWindow view window to define eye of view
  * \param theSketch to convert 3D point coordinates into coorditates of the sketch plane
  */
  static std::shared_ptr<GeomAPI_Pnt2d> getPnt2d(QMouseEvent* theEvent,
                                                 ModuleBase_IViewWindow* theWindow,
                                                 const FeaturePtr& theSketch);

  /** Returns point 2d from selected shape
   *  \param theView a view window
   *  \param theShape a vertex shape
   *  \param theX an output value of X coordinate
   *  \param theY an output value of Y coordinate
   */
  static std::shared_ptr<GeomAPI_Pnt2d> getPnt2d(const Handle(V3d_View)& theView,
                                                 const TopoDS_Shape& theShape,
                                                 const FeaturePtr& theSketch);

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
                               std::string theAttr, QList<bool>& theIsAttributes);

  /*
  * Finds and returns feature reerenced to the paramenter feature with the given name if found
  * \param theFeature a source feature where refsToMe is obtained
  * \param theFeatureId an indentifier of the searched feature
  */
  static FeaturePtr findRefsToMeFeature(FeaturePtr theFeature, const std::string& theFeatureId)
  {
    if (!theFeature.get())
      return FeaturePtr();

    // find first projected feature and edit it
    const std::set<AttributePtr>& aRefsList = theFeature->data()->refsToMe();
    std::set<AttributePtr>::const_iterator anIt;
    for (anIt = aRefsList.cbegin(); anIt != aRefsList.cend(); ++anIt) {
      FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*anIt)->owner());
      if (aRefFeature && aRefFeature->getKind() == theFeatureId)
        return aRefFeature;
    }
    return FeaturePtr();
  }

  /**
  * Returns point of a coincedence
  * \param theStartCoin the coincedence feature
  */
  static std::shared_ptr<GeomAPI_Pnt2d> getCoincedencePoint(FeaturePtr theStartCoin);

  /// Sends redisplay event for all sub-features of the composite. Flush it.
  static void sendSubFeaturesEvent(const CompositeFeaturePtr& theComposite,
                                   const Events_ID theId);

  /**
   * Returns true if the object is a sketch entity, where auxiliary attribute has true value
   * \param theObject a result or feature
   * \return boolean result
   */
  static bool isAuxiliarySketchEntity(const ObjectPtr& theObject);

  /**
   * Returns true if the object is a sketch entity produced by projection or intersection feature
   * and if it should not be included into the sketch result
   */
  static bool isIncludeIntoSketchResult(const ObjectPtr& theObject);

  static ResultPtr createFixedByExternalCenter(const ObjectPtr& theObject,
                                               const std::shared_ptr<GeomAPI_Edge>& theEdge,
                                               ModelAPI_AttributeSelection::CenterType theType,
                                               const CompositeFeaturePtr& theSketch,
                                               bool theTemporary,
                                               FeaturePtr& theCreatedFeature);


  static void getFirstAndLastIndexInFolder(const ObjectPtr& theFolder,
    int& theFirst, int& theLast);


  /**
  * Returns default color value for the given object
  */
  static void getDefaultColor(ObjectPtr theObject, const bool isEmptyColorValid,
    std::vector<int>& theColor);

  /**
  * Returns default deflection value for the given object
  */
  static double getDefaultDeflection(const ObjectPtr& theObject);


  /**
  * Returns default transparency value
  */
  static double getDefaultTransparency();

  /**
  * Returns cursor according to (SKETCH_TAB_NAME, "operation_cursor") property value
  */
  static QCursor getOperationCursor();
};

#endif
