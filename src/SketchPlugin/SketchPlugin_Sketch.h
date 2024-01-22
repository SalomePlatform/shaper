// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef SketchPlugin_Sketch_H_
#define SketchPlugin_Sketch_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_IPresentable.h>
#include <GeomAPI_ICustomPrs.h>

#include <GeomAPI_Ax3.h>
#include <GeomAPI_XYZ.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <list>

#ifdef SET_PLANES_COLOR_IN_PREFERENCES
  #define YZ_PLANE_COLOR "225,0,0"
  #define XZ_PLANE_COLOR "0,225,0"
  #define XY_PLANE_COLOR "0,0,225"
#endif

/**\class SketchPlugin_Sketch
 * \ingroup Plugins
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Sketch : public ModelAPI_CompositeFeature//, public GeomAPI_ICustomPrs
{
 public:
  /// Sketch feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_SKETCH_ID("Sketch");
    return MY_SKETCH_ID;
  }
  /// Origin point of the sketcher in 3D space
  inline static const std::string& ORIGIN_ID()
  {
    static const std::string MY_ORIGIN_ID("Origin");
    return MY_ORIGIN_ID;
  }
  /// Vector X inside of the sketch plane
  inline static const std::string& DIRX_ID()
  {
    static const std::string MY_DIRX_ID("DirX");
    return MY_DIRX_ID;
  }
  /// Vector Z, normal to the sketch plane
  inline static const std::string& NORM_ID()
  {
    static const std::string MY_NORM_ID("Norm");
    return MY_NORM_ID;
  }
  /// All features of this sketch (list of references)
  inline static const std::string& FEATURES_ID()
  {
    static const std::string MY_FEATURES_ID("Features");
    return MY_FEATURES_ID;
  }
  /// Sketch solver error
  inline static const std::string& SOLVER_ERROR()
  {
    static const std::string MY_SOLVER_ERROR("SolverError");
    return MY_SOLVER_ERROR;
  }

  /// Sketch solver error
  inline static const std::string& SOLVER_DOF()
  {
    static const std::string MY_SOLVER_DOF("SolverDOF");
    return MY_SOLVER_DOF;
  }

  /// Action ID to remove links to external entities while changing the sketch plane.
  inline static const std::string& ACTION_REMOVE_EXTERNAL()
  {
    static const std::string MY_ACTION_REMOVE_EXTERNAL("RemoveExternalLinks");
    return MY_ACTION_REMOVE_EXTERNAL;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Sketch::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Converts a 2D sketch space point into point in 3D space
  /// \param theX an X coordinate
  /// \param theY an Y coordinate
  std::shared_ptr<GeomAPI_Pnt> to3D(const double theX, const double theY) const
  {
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        data()->attribute(ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(NORM_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(DIRX_ID()));
    std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));

    std::shared_ptr<GeomAPI_XYZ> aSum = aC->pnt()->xyz()->added(aX->dir()->xyz()->multiplied(theX))
        ->added(aY->xyz()->multiplied(theY));

    return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSum));
  }

  /// Returns the point projected into the sketch plane
  /// \param thePnt a source 3d point
  std::shared_ptr<GeomAPI_Pnt2d> to2D(const std::shared_ptr<GeomAPI_Pnt>& thePnt) const
  {
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        data()->attribute(ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(NORM_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(DIRX_ID()));
    std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));
    return thePnt->to2D(aC->pnt(), aX->dir(), aY);
  }

  /// Returns true if this feature must be displayed in the history (top level of Part tree)
  SKETCHPLUGIN_EXPORT virtual bool isInHistory()
  {
    return true;
  }

  /// Use plugin manager for features creation
  SketchPlugin_Sketch();

  /// Returns the basis plane for the sketch
  std::shared_ptr<GeomAPI_Pln> plane() const
  {
    std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        data()->attribute(ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(NORM_ID()));

    if (anOrigin.get() && aNorm.get() && anOrigin->isInitialized() && aNorm->isInitialized())
      return std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(anOrigin->pnt(), aNorm->dir()));

    return std::shared_ptr<GeomAPI_Pln>();
  }

  /// Returns currently defined plane as an object of Ax3
  std::shared_ptr<GeomAPI_Ax3> coordinatePlane() const
  {
    DataPtr aData = data();
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(DIRX_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(NORM_ID()));

    if (!aNorm->isInitialized() || !aX->isInitialized() ||
        aNorm->dir()->cross(aX->dir())->squareModulus() < 1.e-14)
      return std::shared_ptr<GeomAPI_Ax3>();

    return std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3(aC->pnt(), aX->dir(), aNorm->dir()));
  }

  /// Checks whether the plane is set in the sketch.
  /// \returns the boolean state
  bool isPlaneSet() const
  {
    std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(NORM_ID()));

    return aNormal && !(aNormal->x() == 0 && aNormal->y() == 0 && aNormal->z() == 0);
  }


  /// appends a feature to the sketch sub-elements container
  SKETCHPLUGIN_EXPORT virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);

  /// Just to synchronise the container of sub-features
  virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Returns the number of sub-elements
  SKETCHPLUGIN_EXPORT virtual int numberOfSubs(bool forTree = false) const;

  /// Returns the sub-feature by zero-base index
  SKETCHPLUGIN_EXPORT virtual std::shared_ptr<ModelAPI_Feature>
    subFeature(const int theIndex, bool forTree = false);

  /// Returns the sub-feature unique identifier in this composite feature by index
  SKETCHPLUGIN_EXPORT virtual int subFeatureId(const int theIndex) const;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  SKETCHPLUGIN_EXPORT virtual bool isSub(ObjectPtr theObject) const;

  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Performs some custom feature specific functionality (normally called by some GUI button)
  /// \param theActionId an action key
  /// \return a boolean value about it is performed
  SKETCHPLUGIN_EXPORT virtual bool customAction(const std::string& theActionId);

  /// \brief Create a result for the point in the attribute if the attribute is initialized
  /// \param theFeature a source feature
  /// \param theSketch a sketch intance
  /// \param theAttributeID an attribute string
  /// \param theIndex an index of the result
  static void createPoint2DResult(ModelAPI_Feature* theFeature,
                                  SketchPlugin_Sketch* theSketch,
                                  const std::string& theAttributeID, const int theIndex);

  /// \brief Create a result for the segment given by a pair of attributes
  /// \param theFeature a source feature
  /// \param theSketch a sketch intance
  /// \param theStartAttrID an attribute string
  /// \param theEndAttrID an attribute string
  /// \param theIndex an index of the result
  static void createLine2DResult(ModelAPI_Feature* theFeature,
                                 SketchPlugin_Sketch* theSketch,
                                 const std::string& theStartAttrID,
                                 const std::string& theEndAttrID,
                                 const int theIndex = 0);

  /// Add new feature and fill the data of the feature by the data of the parameter feature.
  /// The name of the created feature stays unique.
  /// \param theFeature a source feature
  /// \param theSketch a sketch intance
  /// \param theIsCopy if true sets feature copy attribute to true.
  /// \return a created feature
  static FeaturePtr addUniqueNamedCopiedFeature(FeaturePtr theFeature,
                                                SketchPlugin_Sketch* theSketch,
                                                const bool theIsCopy = false);

  /// Creates a plane of the sketch.
  /// \param theSketch a sketch intance
  static std::shared_ptr<GeomAPI_Ax3> plane(SketchPlugin_Sketch* theSketch);

  /// Customize presentation of the feature
  //virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
  //                                   std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
  //{
  //  bool isCustomized = false;
  //  // apply the color of the result to the presentation
  //  if (theDefaultPrs.get())
  //    isCustomized = theDefaultPrs->customisePresentation(theResult, thePrs, theDefaultPrs);
  //  // set the sketch presentation bold
  //  isCustomized = thePrs->setWidth(2) || isCustomized;

  //  return isCustomized;
  //}

private:
  /// Substitute all links to external objects by newly created features.
  /// \return \c true, if all links updated.
  bool removeLinksToExternal();

  /// Update projected coordinate axes
  void updateCoordinateAxis(ObjectPtr theSub, std::shared_ptr<GeomAPI_Ax3> thePlane);

private:
  std::shared_ptr<GeomAPI_Ax3> myPlane;
};

#endif
