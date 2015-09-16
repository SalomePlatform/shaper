// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Sketch.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

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

#define YZ_PLANE_COLOR "225,0,0"
#define XZ_PLANE_COLOR "0,225,0"
#define XY_PLANE_COLOR "0,0,225"

/**\class SketchPlugin_Sketch
 * \ingroup Plugins
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Sketch : public ModelAPI_CompositeFeature, public GeomAPI_ICustomPrs//, public GeomAPI_IPresentable
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

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY)
  {
  }

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

    if (!anOrigin || !aNorm)
      return std::shared_ptr<GeomAPI_Pln>();

    return std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(anOrigin->pnt(), aNorm->dir()));
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


  /// removes also all sub-sketch elements
  SKETCHPLUGIN_EXPORT virtual void erase();

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

  /// Construction result is allways recomuted on the fly
  SKETCHPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Exchanges IDs of two given features: needed for fillet feature better naming (issue 769)
  SKETCHPLUGIN_EXPORT virtual void exchangeIDs(std::shared_ptr<ModelAPI_Feature> theFeature1,
    std::shared_ptr<ModelAPI_Feature> theFeature2);


  /// \brief Create a result for the point in the attribute if the attribute is initialized
  /// \param theAttributeID an attribute string
  /// \param theIndex an index of the result
  static void createPoint2DResult(ModelAPI_Feature* theFeature,
                                  SketchPlugin_Sketch* theSketch,
                                  const std::string& theAttributeID, const int theIndex);
  
  /// Add new feature and fill the data of the feature by the data of the parameter feature.
  /// The name of the created feature stays unique.
  /// \param theFeature a source feature
  /// \return a created feature
  static FeaturePtr addUniqueNamedCopiedFeature(FeaturePtr aFeature,
                                                SketchPlugin_Sketch* theSketch);

  /// Creates a plane of the sketch.
  /// \param theSketch a sketch intance
  static std::shared_ptr<GeomAPI_Ax3> plane(SketchPlugin_Sketch* theSketch);

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
  {
    bool isCustomized = false;
    // apply the color of the result to the presentation
    if (theDefaultPrs.get())
      isCustomized = theDefaultPrs->customisePresentation(theResult, thePrs, theDefaultPrs);
    // set the sketch presentation bold    
    isCustomized = thePrs->setWidth(2) || isCustomized;
  
    return isCustomized;
  }

};

#endif
