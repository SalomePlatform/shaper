// File:    SketchPlugin_ConstraintLength.h
// Created: 30 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintLength_H_
#define SketchPlugin_ConstraintLength_H_

#include "SketchPlugin.h"
#include "SketchPlugin_ConstraintBase.h"
#include <SketchPlugin_Sketch.h>
#include <list>

#define LENGTH_COLOR "#ff00ff"

/** \class SketchPlugin_ConstraintLength
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint which defines a length of a line segment
 *
 *  These constraint has two attributes:
 *  SketchPlugin_Constraint::VALUE() (length) and SketchPlugin_Constraint::ENTITY_A() (segment),
 *  SketchPlugin_Constraint::FLYOUT_VALUE_PNT() (distance of a constraints handle)
 */
class SketchPlugin_ConstraintLength : public SketchPlugin_ConstraintBase
{
 public:
  /// Length constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_LENGTH_ID("SketchConstraintLength");
    return MY_CONSTRAINT_LENGTH_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintLength::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintLength();
};

#endif
