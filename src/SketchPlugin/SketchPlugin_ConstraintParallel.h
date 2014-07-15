// File:    SketchPlugin_ConstraintParallel.h
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintParallel_HeaderFile
#define SketchPlugin_ConstraintParallel_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_Constraint.h"

/** \class SketchPlugin_ConstraintParallel
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint parallelism of two lines
 *
 *  These constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintParallel: public SketchPlugin_Constraint
{
public:
  /// Parallel constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_PARALLEL_ID("SketchConstraintParallel");
    return MY_CONSTRAINT_PARALLEL_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = SketchPlugin_ConstraintParallel::ID(); return MY_KIND;}

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
  SketchPlugin_ConstraintParallel();
};

#endif
