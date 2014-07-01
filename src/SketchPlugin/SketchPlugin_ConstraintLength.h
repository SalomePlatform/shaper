// File:    SketchPlugin_ConstraintLength.h
// Created: 30 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintLength_HeaderFile
#define SketchPlugin_ConstraintLength_HeaderFile

#include "SketchPlugin.h"
#include "SketchPlugin_Constraint.h"
#include <SketchPlugin_Sketch.h>
#include <list>

/// Length constraint kind
const std::string SKETCH_CONSTRAINT_LENGTH_KIND("SketchConstraintLength");

/** \class SketchPlugin_ConstraintLength
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint which defines a length of a line segment
 *
 *  These constraint has two attributes:
 *  CONSTRAINT_ATTR_VALUE (length) and CONSTRAINT_ATTR_ENTITY_A (segment),
 *  CONSTRAINT_ATTR_FLYOUT_VALUE_PNT (distance of a constraints handle)
 */
class SketchPlugin_ConstraintLength: public SketchPlugin_Constraint
{
public:
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = SKETCH_CONSTRAINT_LENGTH_KIND; return MY_KIND;}

  /// \brief Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = SKETCH_KIND; return MY_GROUP;}

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual boost::shared_ptr<GeomAPI_AISObject> getAISObject(
                                boost::shared_ptr<GeomAPI_AISObject> thePrevious);

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintLength();
};

#endif
