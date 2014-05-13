// File:    SketchPlugin_ConstraintPointsCoincident.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintPointsCoincident_HeaderFile
#define SketchPlugin_ConstraintPointsCoincident_HeaderFile

#include "SketchPlugin.h"
#include "SketchPlugin_Constraint.h"
#include <list>


/** \class SketchPlugin_ConstraintPointsCoincident
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint which defines equvalence of two points
 *
 *  These constraint has two attributes: CONSTRAINT_ATTR_POINT_A and CONSTRAINT_ATTR_POINT_B
 */
class SketchPlugin_ConstraintPointsCoincident: public SketchPlugin_Constraint
{
public:
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "SketchConstraintPointsCoincident"; return MY_KIND;}

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
  SketchPlugin_ConstraintPointsCoincident();
};

#endif
