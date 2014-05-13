// File:    SketchSolver_ConstraintManager.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintManager_Headerfile
#define SketchSolver_ConstraintManager_Headerfile

#include "SketchSolver.h"

#include <SketchPlugin_Constraint.h>

// Need to be defined before including SolveSpace to avoid additional dependances on Windows platform
#if defined(WIN32) && !defined(HAVE_C99_INTEGER_TYPES)
typedef unsigned int UINT32;
#endif
#include <string.h>
#include <slvs.h>

#include <map>
#include <vector>


// Unknown constraint (for error reporting)
#define SLVS_C_UNKNOWN 0

/** \class   SketchSolver_ConstraintManager
 *  \ingroup DataModel
 *  \brief   Transforms the Constraint feature into the format understandable by SolveSpace library.
 *
 *  Constraints created for SolveSpace library will be divided into the groups.
 *  The division order based on connectedness of the features by the constraints.
 *  The groups may be fused or separated according to the new constraints.
 */
class SketchSolver_ConstraintManager
{
public:
  SketchSolver_ConstraintManager();
  ~SketchSolver_ConstraintManager();

  /** \brief Adds a constraint into the manager
   *  \param[in] theConstraint constraint to be added
   *  \return \c true if the constraint added successfully
   */
  SKETCHSOLVER_EXPORT bool addConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Removes a constraint from the manager
   *  \param[in] theConstraint constraint to be removed
   *  \return \c true if the constraint removed successfully
   */
  SKETCHSOLVER_EXPORT bool removeConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

private:
  class SketchSolver_ConstraintGroup;

  /** \brief Searches list of groups which interact with specified constraint
   *  \param[in]  theConstraint constraint to be found
   *  \param[out] theGroups     list of group indexes interacted with constraint
   */
  void findGroups(boost::shared_ptr<SketchPlugin_Constraint> theConstraint, 
                  std::vector<Slvs_hGroup>&                  theGroupIDs) const;

private:
  std::vector<SketchSolver_ConstraintGroup> myGroups; ///< groups of constraints
};


/** \class   SketchSolver_ConstraintGroup
 *  \ingroup DataModel
 *  \brief   Keeps the group of constraints which based on the same entities
 */
class SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup
{
public:
  SketchSolver_ConstraintGroup();
  ~SketchSolver_ConstraintGroup();

  /// \brief Returns group's unique identifier
  const Slvs_hGroup& getId() const
  {return myID;}

  /** \brief Adds a constraint into the group
   *  \param[in] theConstraint constraint to be added
   *  \return \c true if the constraint added successfully
   */
  bool addConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Removes a constraint into the group
   *  \param[in] theConstraint constraint to be removed
   *  \return \c true if the constraint removed successfully
   */
  bool removeConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Verifies the constraint uses the objects from this group
   *  \param[in] theConstraint constraint for verification of interaction
   *  \return \c true if the constrained objects are used in current group
   */
  bool isInteract(boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const;

protected:
  /** \brief Creates a workplane from the sketch parameters
   *  \param[in] theParams list of the basic parameters of the workplane
   *  \return \c true if success
   */
  bool addWorkplane(std::list< boost::shared_ptr<ModelAPI_Attribute> >& theParams);

  /** \brief Adds an entity into the group
   *  \param[in] theEntity the object of constraint
   *  \return identifier of created entity or 0 if entity was not added
   */
  Slvs_hEntity addEntity(boost::shared_ptr<ModelAPI_Attribute> theEntity);

  /** \brief Adds a parameter into the group
   *  \param[in] theParam parameter to be added
   *  \return identifier of created parameter or 0 if it was not added
   */
  Slvs_hParam addParameter(double theParam);

  /** \brief Compute constraint type according to SolveSpace identifiers
   *  \param[in] theConstraint constraint which type should be determined
   *  \return identifier of constraint type
   */
  int getConstraintType(const boost::shared_ptr<SketchPlugin_Constraint>& theConstraint) const;

private:
  Slvs_hGroup                  myID;            ///< the index of the group
  Slvs_Entity                  myWorkplane;     ///< Workplane for the current group
  std::vector<Slvs_Param>      myParams;        ///< List of parameters of the constraints
  Slvs_hParam                  myParamMaxID;    ///< Actual maximal ID of parameters
  std::vector<Slvs_Entity>     myEntities;      ///< List of entities of the constaints
  Slvs_hEntity                 myEntityMaxID;   ///< Actual maximal ID of entities
  std::vector<Slvs_Constraint> myConstraints;   ///< List of constraints in SolveSpace format
  Slvs_hConstraint             myConstrMaxID;   ///< Actual maximal ID of constraints
  Slvs_System                  myConstrSet;     ///< SolveSpace's set of equations obtained by constraints
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_Constraint> 
                               myConstraintMap; ///< The map between SketchPlugin and SolveSpace constraints
};

#endif
