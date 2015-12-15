// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_IEntityWrapper.h
// Created: 30 Nov 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_IEntityWrapper_H_
#define SketchSolver_IEntityWrapper_H_

#include <SketchSolver.h>
#include <SketchSolver_IParameterWrapper.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_Feature.h>

#include <list>
#include <memory>

/// Types of entities
enum SketchSolver_EntityType {
  ENTITY_UNKNOWN = 0,
  ENTITY_SCALAR,
  ENTITY_POINT,
  ENTITY_NORMAL,
  ENTITY_LINE,
  ENTITY_CIRCLE,
  ENTITY_ARC,
  ENTITY_SKETCH
};

/**
 *  Wrapper providing operations with entities regardless the solver.
 */
class SketchSolver_IEntityWrapper
{
public:
  virtual ~SketchSolver_IEntityWrapper() {}

  /// \brief Return base feature
  const FeaturePtr& baseFeature() const
  { return myBaseFeature; }
  /// \brief Return base attribute
  const AttributePtr& baseAttribute() const
  { return myBaseAttribute; }

  /// \brief Check the feature is basis for this wrapper
  bool isBase(FeaturePtr theFeature) const
  { return myBaseFeature && myBaseFeature == theFeature; }
  /// \brief Check the attribute is basis for this wrapper
  bool isBase(AttributePtr theAttribute) const
  { return myBaseAttribute && myBaseAttribute == theAttribute; }

  /// \brief Assign list of parameters for this entity
  void setParameters(const std::list<ParameterWrapperPtr>& theParams)
  { myParameters = theParams; }
  /// \brief Return list of parameters
  const std::list<ParameterWrapperPtr>& parameters() const
  { return myParameters; }

  /// \brief Assign list of sub-entities
  void setSubEntities(const std::list<std::shared_ptr<SketchSolver_IEntityWrapper> >& theEntities)
  { mySubEntities = theEntities; }
  /// \brief Return list of sub-entities
  const std::list<std::shared_ptr<SketchSolver_IEntityWrapper> >& subEntities() const
  { return mySubEntities; }

  /// \brief Return ID of current entity
  virtual EntityID id() const = 0;

  /// \brief Change group for the entity
  virtual void setGroup(const GroupID& theGroup) = 0;
  /// \brief Return identifier of the group the entity belongs to
  virtual GroupID group() const = 0;

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const = 0;

  /// \brief Verify the feature is used in the entity
  virtual bool isUsed(FeaturePtr theFeature) const = 0;
  /// \brief Verify the attribute is used in the entity
  virtual bool isUsed(AttributePtr theAttribute) const = 0;

  /// \brief Compare current entity with other
  virtual bool isEqual(const std::shared_ptr<SketchSolver_IEntityWrapper>& theOther) = 0;

  /// \brief Update values of parameters of this entity by the parameters of given one
  /// \return \c true if some parameters change their values
  virtual bool update(const std::shared_ptr<SketchSolver_IEntityWrapper>& theOther) = 0;

protected:
  FeaturePtr   myBaseFeature;
  AttributePtr myBaseAttribute;

  std::list<ParameterWrapperPtr>                            myParameters;
  std::list<std::shared_ptr<SketchSolver_IEntityWrapper> >  mySubEntities;
};

typedef std::shared_ptr<SketchSolver_IEntityWrapper> EntityWrapperPtr;

#endif
