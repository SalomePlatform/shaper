// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Constraint.h
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Constraint_H_
#define SketchSolver_Constraint_H_

#include "SketchSolver.h"
#include <SketchSolver_Storage.h>

#include <SketchPlugin_Constraint.h>

#include <ModelAPI_AttributeRefAttr.h>

#include <string>
#include <vector>

class SketchSolver_Group;

/** \class   SketchSolver_Constraint
 *  \ingroup Plugins
 *  \brief   Stores mapping between SketchPlugin and SolveSpace constraints data
 */
class SketchSolver_Constraint
{
protected:
  /// Default constructor
  SketchSolver_Constraint() {}
  /// Constructor based on SketchPlugin constraint
  SketchSolver_Constraint(ConstraintPtr theConstraint);

public:
  virtual ~SketchSolver_Constraint();

  /// \brief Initializes the storage of SolveSpace constraints
  void setStorage(StoragePtr theStorage);
  /// \brief Initializes group ID for this constraint
  void setGroup(SketchSolver_Group* theGroup);

  /// \brief Update constraint
  virtual void update(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Update SketchPlugin attributes using the data obtained from SolveSpace entities
  virtual void refresh();

  /// \brief Returns the type of constraint
  virtual int getType() const = 0;

  /// \brief The constraint is made temporary
  void makeTemporary() const;

  /// \brief Checks the constraint is used by current object
  virtual bool hasConstraint(ConstraintPtr theConstraint) const
  { return theConstraint == myBaseConstraint; }

  /// \brief Return list of SketchPlugin constraints attached to this object
  virtual std::list<ConstraintPtr> constraints() const;

  /// \brief Return identifier of SolveSpace entity relating to the feature
  Slvs_hEntity getId(FeaturePtr theFeature) const;
  /// \brief Return identifier of SolveSpace entity relating to the attribute
  Slvs_hEntity getId(AttributePtr theAttribute) const;

  /// \brief Adds a feature to constraint and create its analogue in SolveSpace
  virtual void addFeature(FeaturePtr theFeature);

  /// \brief Shows error message
  const std::string& error() const
  { return myErrorMsg; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<Slvs_hEntity>& theAttributes);

  /// \brief Verify the attributes of constraint are changed (and constraint need to rebuild)
  /// \param[in] theConstraint constraint, which attributes should be checked (if NULL, the myBaseConstraint is used)
  /// \return \c true if some attributes are changed
  virtual bool checkAttributesChanged(ConstraintPtr theConstraint);

  /// \brief This method is used in derived objects to check consistency of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint()
  {}

  /// \brief Create or change SlveSpace entity according to the given attribute
  /// \param[in]  theAttribute  reference to the entity to be changed
  /// \param[out] theType       type of created entity
  /// \return identifier of created/updated entity
  Slvs_hEntity changeEntity(AttributeRefAttrPtr theAttribute, int& theType);
  /// \brief Create or change SlveSpace entity according to the given attribute
  Slvs_hEntity changeEntity(AttributePtr theAttribute, int& theType);
  /// \brief Create or change SlveSpace entity according to the given feature
  Slvs_hEntity changeEntity(FeaturePtr theFeature, int& theType);

  /// \brief Calculate middle point on the specified entity
  /// \param[in]  theEntity  arc or line
  /// \param[in]  theCoeff   is a value in [0.0, 1.0] which shows the position of the point on the entity (0.0 - start point, 1.0 - end point)
  /// \param[out] theX       X coordinate of middle point
  /// \param[out] theY       Y coordinate of middle point
  void calculateMiddlePoint(const Slvs_Entity& theEntity, double theCoeff,
                            double& theX, double& theY) const;

  /// \brief Removes the links to unused entities
  void cleanRemovedEntities();

  /// \brief Removes last error
  void cleanErrorMsg()
  { myErrorMsg.clear(); }

private:
  /// \brief Sets error, if the attribute is not initialized
  bool isInitialized(AttributePtr theAttribute);

protected:
  SketchSolver_Group* myGroup; ///< the group which contains current constraint
  ConstraintPtr myBaseConstraint; ///< SketchPlugin constraint
  std::vector<Slvs_hConstraint> mySlvsConstraints; ///< list of indices of SolveSpace constraints, together which equivalent to SketchPlugin constraint
  std::map<FeaturePtr, Slvs_hEntity> myFeatureMap; ///< map feature to the entity it represents
  std::map<AttributePtr, Slvs_hEntity> myAttributeMap; ///< map attribute to the corresponding entity
  std::map<AttributePtr, Slvs_hParam> myValueMap; ///< list of attributes, which represents single value (e.g. distance) used in constraint
  StoragePtr myStorage; ///< storage, which contains all information about entities and constraints in current group

  std::string myErrorMsg; ///< error message
};

typedef std::shared_ptr<SketchSolver_Constraint> SolverConstraintPtr;



/** \class   SketchSolver_ConstraintParallel
 *  \ingroup Plugins
 *  \brief   Convert Parallel constraint to SolveSpace structure
 */
class SketchSolver_ConstraintParallel : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintParallel(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return SLVS_C_PARALLEL; }
};


/** \class   SketchSolver_ConstraintPerpendicular
 *  \ingroup Plugins
 *  \brief   Convert Perpendicular constraint to SolveSpace structure
 */
class SketchSolver_ConstraintPerpendicular : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintPerpendicular(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return SLVS_C_PERPENDICULAR; }
};


/** \class   SketchSolver_ConstraintHorizontal
 *  \ingroup Plugins
 *  \brief   Convert Horizontal constraint to SolveSpace structure
 */
class SketchSolver_ConstraintHorizontal : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintHorizontal(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return SLVS_C_HORIZONTAL; }
};


/** \class   SketchSolver_ConstraintVertical
 *  \ingroup Plugins
 *  \brief   Convert Vertical constraint to SolveSpace structure
 */
class SketchSolver_ConstraintVertical : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintVertical(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return SLVS_C_VERTICAL; }
};


/** \class   SketchSolver_ConstraintRadius
 *  \ingroup Plugins
 *  \brief   Convert Radius constraint to SolveSpace structure
 */
class SketchSolver_ConstraintRadius : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintRadius(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return SLVS_C_DIAMETER; }

  virtual void adjustConstraint()
  {
    Slvs_Constraint aConstraint = myStorage->getConstraint(mySlvsConstraints.front());
    aConstraint.valA *= 2.0;
    myStorage->updateConstraint(aConstraint);
  }
};

#endif
