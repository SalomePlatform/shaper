// File:    SketchSolver_ConstraintManager.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintManager_Headerfile
#define SketchSolver_ConstraintManager_Headerfile

#include "SketchSolver.h"

#include <Events_Listener.h>
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
 *  \brief   Listens the changes of SketchPlugin features and transforms the Constraint 
 *           feature into the format understandable by SolveSpace library.
 *
 *  Constraints created for SolveSpace library are divided into the groups.
 *  The division order based on connectedness of the features by the constraints.
 *  The groups may be fused or separated according to the new constraints.
 *
 *  \remark This is a singleton.
 */
class SketchSolver_ConstraintManager : public Events_Listener
{
public:
  /** \brief Main method to create constraint manager
   *  \return pointer to the singleton
   */
  static SketchSolver_ConstraintManager* Instance();

  /** \brief Implementation of Event Listener method
   *  \param[in] theMessage the data of the event
   */
  virtual void processEvent(const Events_Message* theMessage);

protected:
  SketchSolver_ConstraintManager();
  ~SketchSolver_ConstraintManager();

  /** \brief Adds a constraint into the manager
   *  \param[in] theConstraint constraint to be added
   *  \return \c true if the constraint added successfully
   */
  bool addConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Removes a constraint from the manager
   *  \param[in] theConstraint constraint to be removed
   *  \return \c true if the constraint removed successfully
   */
  bool removeConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Updates a constraint
   *  \param[in] theConstraint constraint to be updated
   *  \return \c true if the constraint was updated
   */
  bool updateConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Adds a workplane into the manager
   *  \param[in] theSketch the feature to create workplane
   *  \return \c true if the workplane added successfully
   */
  bool addWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theSketch);

  /** \brief Removes a workplane from the manager. 
   *         All groups based on such workplane will be removed too.
   *  \param[in] theSketch the feature to be removed
   *  \return \c true if the workplane removed successfully
   */
  bool removeWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theSketch);

  /** \brief Updates a workplane
   *  \param[in] theSketch workplane to be updated
   *  \return \c true if the workplane was updated
   */
  bool updateWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theSketch);

  /** \brief Updates entity which is neither workplane nor constraint
   *  \param[in] theFeature entity to be updated
   *  \return \c true if the entity updated successfully
   */
  bool updateEntity(boost::shared_ptr<SketchPlugin_Feature> theFeature);

private:
  class SketchSolver_ConstraintGroup;

  /** \brief Searches list of groups which interact with specified constraint
   *  \param[in]  theConstraint constraint to be found
   *  \param[out] theGroups     list of group indexes interacted with constraint
   */
  void findGroups(boost::shared_ptr<SketchPlugin_Constraint> theConstraint, 
                  std::vector<Slvs_hGroup>&                  theGroupIDs) const;

  /** \brief Searches in the list of groups the workplane which constains specified constraint
   *  \param[in] theConstraint constraint to be found
   *  \return workplane contains the constraint
   */
  boost::shared_ptr<SketchPlugin_Sketch> findWorkplaneForConstraint(
                  boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const;

private:
  static SketchSolver_ConstraintManager*    _self;    ///< Self pointer to implement singleton functionality
  std::vector<SketchSolver_ConstraintGroup> myGroups; ///< Groups of constraints
};


/** \class   SketchSolver_ConstraintGroup
 *  \ingroup DataModel
 *  \brief   Keeps the group of constraints which based on the same entities
 */
class SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup
{
public:
  /** \brief New group based on specified workplane
   */
  SketchSolver_ConstraintGroup(boost::shared_ptr<SketchPlugin_Sketch> theWorkplane);

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

  /** \brief Verifies the specified workplane is the same as a base workplane for this group
   *  \param[in] theWorkplane workplane to be compared
   *  \return \c true if workplanes are the same
   */
  bool isBaseWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theWorkplane) const;

  boost::shared_ptr<SketchPlugin_Sketch> getWorkplane() const
  { return mySketch; }

protected:
  /** \brief Adds an entity into the group
   *
   *  The parameters of entity will be parsed and added to the list of SolveSpace parameters.
   *  Parameters of certain entity will be placed sequentially in the list.
   *
   *  \param[in] theEntity the object of constraint
   *  \return identifier of created entity or 0 if entity was not added
   */
  Slvs_hEntity addEntity(boost::shared_ptr<ModelAPI_Attribute> theEntity);

  /** \brief Adds a normal into the group
   *
   *  Normal is a special entity in SolveSpace, which defines a direction in 3D and 
   *  a rotation about this direction. So, SolveSpace represents normals as unit quaternions.
   *
   *  To define a normal there should be specified two coordinate axis 
   *  on the plane transversed to created normal.
   *
   *  \param[in] theDirX first coordinate axis of the plane
   *  \param[in] theDirY second coordinate axis of the plane
   *  \return identifier of created normal
   */
  Slvs_hEntity addNormal(boost::shared_ptr<ModelAPI_Attribute> theDirX, 
                         boost::shared_ptr<ModelAPI_Attribute> theDirY);

  /** \brief Adds a parameter into the group
   *  \param[in] theParam parameter to be added
   *  \return identifier of created parameter or 0 if it was not added
   */
  Slvs_hParam addParameter(double theParam);

  /** \brief Compute constraint type according to SolveSpace identifiers
   *         and verify that constraint parameters are correct
   *  \param[in] theConstraint constraint which type should be determined
   *  \return identifier of constraint type or SLVS_C_UNKNOWN if the type is wrong
   */
  int getConstraintType(const boost::shared_ptr<SketchPlugin_Constraint>& theConstraint) const;

private:
  /** \brief Creates a workplane from the sketch parameters
   *  \param[in] theSketch parameters of workplane are the attributes of this sketch
   *  \return \c true if success
   */
  bool addWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theSketch);

private:
  // SolveSpace entities
  Slvs_hGroup                  myID;            ///< the index of the group
  Slvs_Entity                  myWorkplane;     ///< Workplane for the current group
  std::vector<Slvs_Param>      myParams;        ///< List of parameters of the constraints
  Slvs_hParam                  myParamMaxID;    ///< Actual maximal ID of parameters (not equal to myParams size)
  std::vector<Slvs_Entity>     myEntities;      ///< List of entities of the constaints
  Slvs_hEntity                 myEntityMaxID;   ///< Actual maximal ID of entities (not equal to myEntities size)
  std::vector<Slvs_Constraint> myConstraints;   ///< List of constraints in SolveSpace format
  Slvs_hConstraint             myConstrMaxID;   ///< Actual maximal ID of constraints (not equal to myConstraints size)
  Slvs_System                  myConstrSet;     ///< SolveSpace's set of equations obtained by constraints

  // SketchPlugin entities
  boost::shared_ptr<SketchPlugin_Sketch> mySketch; ///< Equivalent to workplane
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_Constraint> 
                               myConstraintMap; ///< The map between SketchPlugin and SolveSpace constraints
};

#endif
