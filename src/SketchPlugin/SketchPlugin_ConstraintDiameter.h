// File:    SketchPlugin_ConstraintDiameter.h
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintDiameter_HeaderFile
#define SketchPlugin_ConstraintDiameter_HeaderFile

#include "SketchPlugin.h"
#include "SketchPlugin_Constraint.h"


/** \class SketchPlugin_ConstraintDiameter
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint which defines a diameter of a circle
 *
 *  These constraint has two attributes:
 *  CONSTRAINT_ATTR_VALUE (diameter), CONSTRAINT_ATTR_ENTITY_A (a circle)
 */
class SketchPlugin_ConstraintDiameter: public SketchPlugin_Constraint
{
public:
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "SketchConstraintDiameter"; return MY_KIND;}

  /// \brief Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = "Sketch"; return MY_GROUP;}

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// \brief Returns the sketch preview
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview();

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintDiameter();
};

#endif
