// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_PointWrapper.h
// Created: 16 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_PointWrapper_H_
#define PlaneGCSSolver_PointWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <SketchSolver_IEntityWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS points.
 */
class PlaneGCSSolver_PointWrapper : public SketchSolver_IEntityWrapper
{
public:
  PlaneGCSSolver_PointWrapper(const AttributePtr theAttribute, const GCSPointPtr thePoint);

  /// \brief Return PlaneGCS point
  const GCSPointPtr& point() const
  { return myPoint; }
  /// \brief Return PlaneGCS point to change
  GCSPointPtr& changeEntity()
  { return myPoint; }

  /// \brief Return ID of current entity
  virtual EntityID id() const
  { return myID; }
  /// \brief Change ID of the entity
  void setId(EntityID theID)
  { myID = theID; }

  /// \brief Change group for the entity
  virtual void setGroup(const GroupID& theGroup);
  /// \brief Return identifier of the group the entity belongs to
  virtual GroupID group() const
  { return myGroup; }

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return ENTITY_POINT; }

  /// \brief Verify the feature is used in the entity
  virtual bool isUsed(FeaturePtr theFeature) const
  { return false; }
  /// \brief Verify the attribute is used in the entity
  virtual bool isUsed(AttributePtr theAttribute) const;

  /// \brief Compare current entity with other
  virtual bool isEqual(const EntityWrapperPtr& theOther);

  /// \brief Update values of parameters of this entity by the parameters of given one
  /// \return \c true if some parameters change their values
  virtual bool update(const EntityWrapperPtr& theOther);

private:
  EntityID    myID;
  GroupID     myGroup;
  GCSPointPtr myPoint;
};

#endif
