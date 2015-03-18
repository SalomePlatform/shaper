// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_FeatureStorage.h
// Created: 23 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_FeatureStorage_H_
#define SketchSolver_FeatureStorage_H_

#include <SketchSolver.h>
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Constraint.h>

#include <set>
#include <map>

typedef std::map<FeaturePtr, std::set<ConstraintPtr> > MapFeatureConstraint;
typedef std::map<AttributePtr, std::set<FeaturePtr> >  MapAttributeFeature;

/** \class   SketchSolver_FeatureStorage
 *  \ingroup Plugins
 *  \brief   Collects information about SketchPlugin constraints used in specific group
 */
class SketchSolver_FeatureStorage
{
public:
  SketchSolver_FeatureStorage() {}

  /// \brief Adds or changes a constraint and all features it uses in the storage
  void changeConstraint(ConstraintPtr theConstraint);
  /// \brief Removes a constraint and all its features not used by other constraints
  void removeConstraint(ConstraintPtr theConstraint);
  /// \brief Verifies a constraint is used in the current storage
  bool isInteract(ConstraintPtr theConstraint) const;

  /// \brief Adds or changes a feature in the storage
  void changeFeature(FeaturePtr theFeature);
  /// \brief Adds or changes a feature in the storage. The feature is used in specified constraint
  void changeFeature(FeaturePtr theFeature, ConstraintPtr theConstraint);
  /// \brief Removes a feature
  void removeFeature(FeaturePtr theFeature);
  /// \brief Removes a feature according to a given constraint
  void removeFeature(FeaturePtr theFeature, ConstraintPtr theConstraint);
  /// \brief Verifies a feature is used in the current storage
  bool isInteract(FeaturePtr theFeature) const;

  /// \brief Adds or changes an attribute in the storage
  void changeAttribute(AttributePtr theAttribute);
  /// \brief Adds or changes a attribute in the storage.
  ///        The attribute is used in specified feature or constraint (theFeature)
  void changeAttribute(AttributePtr theAttribute, FeaturePtr theFeature);
  /// \brief Removes an attribute
  void removeAttribute(AttributePtr theAttribute);
  /// \brief Removes an attribute according to a given feature
  void removeAttribute(AttributePtr theAttribute, FeaturePtr theFeature);
  /// \brief Verifies an attribute is used in the current storage
  bool isInteract(AttributePtr theAttribute) const;

  /// \brief Check the features is not removed
  bool isConsistent() const;

  /// \brief Prepares list of constraints, which using specified feature or its attributes
  std::set<ConstraintPtr> getConstraints(FeaturePtr theFeature) const;
  /// \brief Prepares list of constraints, which using specified attribute
  std::set<ConstraintPtr> getConstraints(AttributePtr theAttribute) const;

private:
  std::set<ConstraintPtr> myConstraints; ///< list of SketchPlugin constraints used in the current group
  MapFeatureConstraint myFeatures; ///< list of features used in the group and corresponding constraints which use the feature
  MapAttributeFeature myAttributes; ///< list of attributes used in the group and corresponding features which are based on the attribute
};

typedef std::shared_ptr<SketchSolver_FeatureStorage> FeatureStoragePtr;

#endif // SketchSolver_FeatureStorage_H_
