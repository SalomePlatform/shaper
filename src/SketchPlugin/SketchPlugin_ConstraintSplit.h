// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintSplit.h
// Created: 19 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintSplit_H_
#define SketchPlugin_ConstraintSplit_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

class GeomDataAPI_Point2D;
class ModelAPI_Feature;

/** \class SketchPlugin_ConstraintSplit
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint filleting two objects which have coincident point
 *
 *  This constraint has three attributes:
 *  SketchPlugin_Constraint::VALUE() contains reference object to be splitted
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B() for the points of split;
 *
 *  Also the constraint has attribute SketchPlugin_Constraint::ENTITY_C()
 *  which contains created list objects forming the fillet
 */
class SketchPlugin_ConstraintSplit : public SketchPlugin_ConstraintBase
{
 public:
   /*struct FilletFeatures {
    std::list<std::pair<FeaturePtr, bool>> baseEdgesState; ///< list of objects the fillet is based and its states
    std::list<FeaturePtr> resultEdges; ///< list of result edges
    std::list<FeaturePtr> resultConstraints; ///< list of constraints provided by the fillet
   };*/

  /// Split constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_SPLIT_ID("SketchConstraintSplit");
    return MY_CONSTRAINT_SPLIT_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintSplit::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  //SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const;

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintSplit();

  /// \return map of base points and features;
  //SKETCHPLUGIN_EXPORT const std::map<AttributePtr, FilletFeatures> pointsFeaturesMap() const {
  //  return myPointFeaturesMap;
  //};

private:
  /// Returns geom point attribute of the feature
  /// \param theStartPoint
  /// \return geometical point
  std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(const bool& theStartPoint);

  /// Returns cast of attribute to geometrical point if the attribute is a ref attr attribute
  /// \param theAttribute an attribute
  /// \param geom point 2D or NULL
  std::shared_ptr<GeomDataAPI_Point2D> getPointOfRefAttr(const AttributePtr& theAttribute);

  /// Creates a new feature in the base shape type with bounding points given in parameters
  /// \param theStartPointAttr an attribute of the start point
  /// \param theEndPointAttr an attribute of the end point
  std::shared_ptr<ModelAPI_Feature> FeaturePtr SketchPlugin_ConstraintSplit::createFeature(
                                    const std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
                                    const std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr);

private:
  //std::set<AttributePtr> myNewPoints; ///< set of new points
  //std::map<AttributePtr, FilletFeatures> myPointFeaturesMap; ///< map of point and features for fillet
  //bool myListOfPointsChangedInCode; ///< flag to track that list of points changed in code
  //bool myRadiusChangedByUser; ///< flag to track that radius changed by user
  //bool myRadiusChangedInCode; ///< flag to track that radius changed in code
  //bool myRadiusInitialized; /// < flag to track that radius initialized
};

#endif
