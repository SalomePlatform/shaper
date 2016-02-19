// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Arc.h
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#ifndef SketchPlugin_Arc_H_
#define SketchPlugin_Arc_H_

#include "SketchPlugin.h"
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Sketch.h>
#include <GeomAPI_IPresentable.h>

/**\class SketchPlugin_Arc
 * \ingroup Plugins
 * \brief Feature for creation of the new arc of circle in PartSet.
 * The visualization of this object is separated in two parts. The first one is an AIS object
 * calculated when there is non-initialized attributes of the arc. The second is a result and
 * it is calculated if all attributes are initialized.
 */
class SketchPlugin_Arc : public SketchPlugin_SketchEntity, public GeomAPI_IPresentable
{
  /// to avoid cyclic dependencies in automatic updates: they mean that 
  /// update is performed right now and automatic updates are not needed
  bool myStartUpdate, myEndUpdate;
  /// to avoid (if possible) additional modification of changed coordinate (issue #855)
  double myXEndBefore, myYEndBefore;

  /// to define in which direction draw arc
  double myParamBefore;

 public:
  /// Arc feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_SKETCH_ARC_ID("SketchArc");
    return MY_SKETCH_ARC_ID;
  }

  inline static const std::string& ARC_TYPE()
  {
    static const std::string TYPE("ArcType");
    return TYPE;
  }

  inline static const std::string& ARC_TYPE_TANGENT()
  {
    static const std::string TYPE("Tangent");
    return TYPE;
  }

  static const std::string& TANGENT_POINT_ID()
  {
    static const std::string TANGENT_PNT("ArcTangentPoint");
    return TANGENT_PNT;
  }

  /// Central 2D point of the circle which contains the arc
  inline static const std::string& CENTER_ID()
  {
    static const std::string MY_CENTER_ID = "ArcCenter";
    return MY_CENTER_ID;
  }
  /// Start 2D point of the arc
  inline static const std::string& START_ID()
  {
    static const std::string MY_START_ID = "ArcStartPoint";
    return MY_START_ID;
  }
  /// End 2D point of the arc
  inline static const std::string& END_ID()
  {
    static const std::string MY_END_ID = "ArcEndPoint";
    return MY_END_ID;
  }

  /// Inversed flag
  inline static const std::string& INVERSED_ID()
  {
    static const std::string MY_INVERSED_ID("InversedArc");
    return MY_INVERSED_ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Arc::ID();
    return MY_KIND;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Creates an arc-shape
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Updates the "reversed" flag
  /// \param isReversed  whether the arc will be reversed
  void setReversed(bool isReversed);
  /// Returns \c true is the arc is reversed
  bool isReversed();

  /// Use plugin manager for features creation
  SketchPlugin_Arc();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// Returns true if all obligatory attributes are initialized
  bool isFeatureValid();

  /// Update attributes like passed point, radius and angle of the arc
  void updateDependentAttributes();

  /// Compose constraints to build tangency arc
  void tangencyArcConstraints();

  /// Project end point of arc to the circle
  void projectEndPoint();
};

#endif
