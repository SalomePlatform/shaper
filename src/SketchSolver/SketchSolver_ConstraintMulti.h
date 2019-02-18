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

#ifndef SketchSolver_ConstraintMulti_H_
#define SketchSolver_ConstraintMulti_H_

#include <SketchSolver_Constraint.h>

#include <vector>

/** \class   SketchSolver_ConstraintMulti
 *  \ingroup Plugins
 *  \brief   Common base class for the Multi constraints
 */
class SketchSolver_ConstraintMulti : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMulti(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myNumberOfObjects(0),
      myNumberOfCopies(0),
      myIsFullValue(false),
      myAdjusted(false),
      myIsEventsBlocked(false),
      myIsProcessingNotify(false)
  {}

  /// \brief Update constraint
  virtual void update();

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr& theFeature, PlaneGCSSolver_Update*);

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin
  /// constraints (like for multiple coincidence)
  virtual bool remove();

  /// \brief Block or unblock events from this constraint
  virtual void blockEvents(bool isBlocked);

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process()
  { /* do nothing here */ }

  /// \brief Collect entities which are translated or rotated (not their copies)
  void getEntities(std::list<EntityWrapperPtr>& theEntities);

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  virtual void getAttributes(EntityWrapperPtr&, std::vector<EntityWrapperPtr>&)
  { /* do nothing here */ }

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

  /// \brief Update parameters of derived classes
  virtual void updateLocal() = 0;

  /// \brief Returns name of NUMBER_OF_COPIES parameter for corresponding feature
  virtual const std::string& nameNbObjects() = 0;

protected:
  /// \brief Convert absolute coordinates to relative coordinates
  virtual void getRelative(double theAbsX, double theAbsY, double& theRelX, double& theRelY) = 0;
  /// \brief Convert relative coordinates to absolute coordinates
  virtual void getAbsolute(double theRelX, double theRelY, double& theAbsX, double& theAbsY) = 0;
  /// \brief Apply transformation for relative coordinates
  virtual void transformRelative(double& theX, double& theY) = 0;

protected:
  int myNumberOfObjects; ///< number of previous initial objects
  int myNumberOfCopies;  ///< number of previous copies of initial objects
  bool myIsFullValue;    ///< value whether the angle/distance is a full or single for objects

  bool myAdjusted; ///< the constraint is already adjusted (to not do it several times)

  /// lists of original features and their copies to find whether some of them are disappeared
  std::set<FeaturePtr> myOriginalFeatures;
  std::set<FeaturePtr> myCopiedFeatures;

  bool myIsEventsBlocked;
  bool myIsProcessingNotify; ///< the notification is already processing
};

#endif
