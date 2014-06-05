// File:    SketchPlugin_ConstraintPerpendicular.h
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintPerpendicular_HeaderFile
#define SketchPlugin_ConstraintPerpendicular_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_Constraint.h"

/// Perpendicular constraint kind
const std::string SKETCH_CONSTRAINT_PERPENDICULAR_KIND("SketchConstraintPerpendicular");

/** \class SketchPlugin_ConstraintPerpendicular
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint for perpendicularity of two lines
 *
 *  These constraint has two attributes:
 *  CONSTRAINT_ATTR_ENTITY_A and CONSTRAINT_ATTR_ENTITY_B
 */
class SketchPlugin_ConstraintPerpendicular: public SketchPlugin_Constraint
{
public:
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = SKETCH_CONSTRAINT_PERPENDICULAR_KIND; return MY_KIND;}

  /// \brief Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = SKETCH_KIND; return MY_GROUP;}

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// \brief Returns the sketch preview
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview();

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintPerpendicular();
};

#endif
