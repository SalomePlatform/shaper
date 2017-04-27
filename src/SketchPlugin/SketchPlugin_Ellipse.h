// Copyright (C) 2017-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Ellipse.h
// Created:     26 April 2017
// Author:      Artem ZHIDKOV

#ifndef SketchPlugin_Ellipse_H_
#define SketchPlugin_Ellipse_H_

#include <SketchPlugin.h>
#include <SketchPlugin_SketchEntity.h>

/**\class SketchPlugin_Ellipse
 * \ingroup Plugins
 * \brief Feature for creation of the new ellipse in Sketch.
 */
class SketchPlugin_Ellipse: public SketchPlugin_SketchEntity
{
 public:
  /// Ellipse feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchEllipse");
    return ID;
  }

  /// 2D point - center of the ellipse
  inline static const std::string& CENTER_ID()
  {
    static const std::string ID("ellipse_center");
    return ID;
  }

  /// 2D point - focus of the ellipse
  inline static const std::string& FOCUS_ID()
  {
    static const std::string ID("ellipse_focus");
    return ID;
  }

  /// Major radius of the ellipse
  inline static const std::string& MAJOR_RADIUS_ID()
  {
    static const std::string ID("ellipse_major_radius");
    return ID;
  }

  /// Minor radius of the ellipse
  inline static const std::string& MINOR_RADIUS_ID()
  {
    static const std::string ID("ellipse_minor_radius");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Ellipse::ID();
    return MY_KIND;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Use plugin manager for features creation
  SketchPlugin_Ellipse();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();
};

#endif
