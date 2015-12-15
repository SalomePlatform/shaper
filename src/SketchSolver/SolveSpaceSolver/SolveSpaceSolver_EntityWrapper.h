// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_EntityWrapper.h
// Created: 2 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef SolveSpaceSolver_EntityWrapper_H_
#define SolveSpaceSolver_EntityWrapper_H_

#include <SketchSolver_IEntityWrapper.h>
#include <SolveSpaceSolver_Solver.h>

/**
 *  Wrapper providing operations with SolveSpace entities.
 */
class SolveSpaceSolver_EntityWrapper : public SketchSolver_IEntityWrapper
{
public:
  SolveSpaceSolver_EntityWrapper(const FeaturePtr theFeature, const Slvs_Entity& theEntity);
  SolveSpaceSolver_EntityWrapper(const AttributePtr theAttribute, const Slvs_Entity& theEntity);

  /// \brief Return SolveSpace entity
  const Slvs_Entity& entity() const
  { return myEntity; }
  /// \brief Return SolveSpace entity to change
  Slvs_Entity& changeEntity()
  { return myEntity; }

  /// \brief Return ID of current entity
  virtual EntityID id() const;

  /// \brief Change group for the entity
  virtual void setGroup(const GroupID& theGroup);
  /// \brief Return identifier of the group the entity belongs to
  virtual GroupID group() const
  { return (GroupID)myEntity.group; }

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const;

  /// \brief Verify the feature is used in the entity
  virtual bool isUsed(FeaturePtr theFeature) const;
  /// \brief Verify the attribute is used in the entity
  virtual bool isUsed(AttributePtr theAttribute) const;

  /// \brief Compare current entity with other
  virtual bool isEqual(const EntityWrapperPtr& theOther);

  /// \brief Update values of parameters of this entity by the parameters of given one
  /// \return \c true if some parameters change their values
  virtual bool update(const std::shared_ptr<SketchSolver_IEntityWrapper>& theOther);

private:
  Slvs_Entity  myEntity;
};

#endif
