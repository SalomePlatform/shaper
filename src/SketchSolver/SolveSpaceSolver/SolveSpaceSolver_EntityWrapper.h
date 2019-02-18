// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

  /// \brief Constructor useful for SketchPlugin_Point only
  SolveSpaceSolver_EntityWrapper(const FeaturePtr thePointFeature,
                                 const AttributePtr thePointAttribute,
                                 const Slvs_Entity& theEntity);

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
