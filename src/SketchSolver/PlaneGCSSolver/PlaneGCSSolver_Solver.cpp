// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <PlaneGCSSolver_Solver.h>
#include <Events_LongOp.h>


PlaneGCSSolver_Solver::PlaneGCSSolver_Solver()
  : myEquationSystem(new GCS::System),
    myDiagnoseBeforeSolve(false),
    myInitilized(false),
    myConfCollected(false),
    myDOF(0),
    myFictiveConstraint(0)
{
}

PlaneGCSSolver_Solver::~PlaneGCSSolver_Solver()
{
  clear();
}

void PlaneGCSSolver_Solver::clear()
{
  myEquationSystem->clear();
  myParameters.clear();
  myConstraints.clear();
  myConflictingIDs.clear();
  myDOF = 0;

  removeFictiveConstraint();
}

void PlaneGCSSolver_Solver::addConstraint(GCSConstraintPtr theConstraint)
{
  myEquationSystem->addConstraint(theConstraint.get());
  myConstraints[theConstraint->getTag()].insert(theConstraint);
  if (theConstraint->getTag() >= 0)
    myDOF = -1;
  myInitilized = false;
}

void PlaneGCSSolver_Solver::removeConstraint(ConstraintID theID)
{
  myConstraints.erase(theID);
  if (myConstraints.empty()) {
    myEquationSystem->clear();
    myDOF = (int)myParameters.size();
  } else {
    myEquationSystem->clearByTag(theID);
    if (theID >= 0)
      myDOF = -1;
  }
  myInitilized = false;
}

double* PlaneGCSSolver_Solver::createParameter()
{
  double* aResult = new double(0);
  myParameters.push_back(aResult);
  if (myConstraints.empty() && myDOF >= 0)
    ++myDOF; // calculate DoF by hand if and only if there is no constraints yet
  else
    myDiagnoseBeforeSolve = true;
  return aResult;
}

void PlaneGCSSolver_Solver::addParameters(const GCS::SET_pD& theParams)
{
  GCS::SET_pD aParams(theParams);
  // leave new parameters only
  GCS::VEC_pD::iterator anIt = myParameters.begin();
  for (; anIt != myParameters.end(); ++anIt)
    if (aParams.find(*anIt) != aParams.end())
      aParams.erase(*anIt);

  myParameters.insert(myParameters.end(), aParams.begin(), aParams.end());
  if (myConstraints.empty() && myDOF >=0)
    myDOF += (int)aParams.size(); // calculate DoF by hand only if there is no constraints yet
  else
    myDiagnoseBeforeSolve = true;
}

void PlaneGCSSolver_Solver::removeParameters(const GCS::SET_pD& theParams)
{
  for (int i = (int)myParameters.size() - 1; i >= 0; --i)
    if (theParams.find(myParameters[i]) != theParams.end()) {
      myParameters.erase(myParameters.begin() + i);
      --myDOF;
    }
  if (!myConstraints.empty())
    myDiagnoseBeforeSolve = true;
}

void PlaneGCSSolver_Solver::initialize()
{
  Events_LongOp::start(this);
  addFictiveConstraintIfNecessary();
  if (myDiagnoseBeforeSolve)
    diagnose();
  myEquationSystem->declareUnknowns(myParameters);
  myEquationSystem->initSolution();
  Events_LongOp::end(this);

  myInitilized = true;
}

PlaneGCSSolver_Solver::SolveStatus PlaneGCSSolver_Solver::solve()
{
  // clear list of conflicting constraints
  if (myConfCollected) {
    myConflictingIDs.clear();
    myConfCollected = false;
  }

  if (myParameters.empty())
    return myConstraints.empty() ? STATUS_OK : STATUS_INCONSISTENT;

  GCS::SolveStatus aResult = GCS::Success;
  Events_LongOp::start(this);
  if (myInitilized) {
    aResult = (GCS::SolveStatus)myEquationSystem->solve();
  } else {
    addFictiveConstraintIfNecessary();
    diagnose();
    aResult = (GCS::SolveStatus)myEquationSystem->solve(myParameters);
  }
  Events_LongOp::end(this);

  // collect information about conflicting constraints every time,
  // sometimes solver reports about succeeded recalculation but has conflicting constraints
  // (for example, apply horizontal constraint for a copied feature)
  collectConflicting();
  if (!myConflictingIDs.empty())
    aResult = GCS::Failed;
  else if (aResult == GCS::Failed) {
    // DogLeg solver failed without conflicting constraints, try to use Levenberg-Marquardt solver
    // if there are point-line distance constraints
    ConstraintMap::iterator aCIt = myConstraints.begin();
    for (; aCIt != myConstraints.end(); ++aCIt) {
      if (aCIt->second.size() <= 1)
        continue;
      std::set<GCSConstraintPtr>::const_iterator anIt = aCIt->second.begin();
      for (; anIt != aCIt->second.end(); ++anIt)
        if ((*anIt)->getTypeId() == GCS::P2LDistance)
          break;
      if (anIt != aCIt->second.end())
        break;
    }

    if (aCIt != myConstraints.end()) {
      aResult = (GCS::SolveStatus)myEquationSystem->solve(
          myParameters, true, GCS::LevenbergMarquardt);
      myConfCollected = false;
      collectConflicting();
      if (!myConflictingIDs.empty())
        aResult = GCS::Failed;
    }
  }

  SolveStatus aStatus;
  if (aResult == GCS::Failed)
    aStatus = STATUS_FAILED;
  else {
    myEquationSystem->applySolution();
    if (myDOF < 0)
      myDOF = myEquationSystem->dofsNumber();
    aStatus = STATUS_OK;
  }

  removeFictiveConstraint();
  myInitilized = false;
  return aStatus;
}

void PlaneGCSSolver_Solver::undo()
{
  myEquationSystem->undoSolution();
}

bool PlaneGCSSolver_Solver::isConflicting(const ConstraintID& theConstraint) const
{
  if (!myConfCollected)
    const_cast<PlaneGCSSolver_Solver*>(this)->collectConflicting();
  return myConflictingIDs.find((int)theConstraint) != myConflictingIDs.end();
}

void PlaneGCSSolver_Solver::collectConflicting()
{
  GCS::VEC_I aConflict;
  myEquationSystem->getConflicting(aConflict);
  myConflictingIDs.insert(aConflict.begin(), aConflict.end());

  myEquationSystem->getRedundant(aConflict);
  myConflictingIDs.insert(aConflict.begin(), aConflict.end());

  myConfCollected = true;
}

int PlaneGCSSolver_Solver::dof()
{
  if (myDOF < 0 && !myConstraints.empty())
    diagnose();
  return myDOF;
}

void PlaneGCSSolver_Solver::diagnose()
{
  myEquationSystem->declareUnknowns(myParameters);
  myDOF = myEquationSystem->diagnose();
  myDiagnoseBeforeSolve = false;
}

void PlaneGCSSolver_Solver::addFictiveConstraintIfNecessary()
{
  if (!myConstraints.empty() &&
      myConstraints.find(CID_MOVEMENT) == myConstraints.end())
    return;

  if (myFictiveConstraint)
    return; // no need several fictive constraints

  double* aParam = createParameter();
  double* aFictiveParameter = new double(0.0);

  myFictiveConstraint = new GCS::ConstraintEqual(aFictiveParameter, aParam);
  myFictiveConstraint->setTag(CID_FICTIVE);
  myEquationSystem->addConstraint(myFictiveConstraint);
}

void PlaneGCSSolver_Solver::removeFictiveConstraint()
{
  if (myFictiveConstraint) {
    myEquationSystem->removeConstraint(myFictiveConstraint);
    myParameters.pop_back();
    --myDOF;

    GCS::VEC_pD aParams = myFictiveConstraint->params();
    for (GCS::VEC_pD::iterator anIt = aParams.begin(); anIt != aParams.end(); ++ anIt)
      delete *anIt;
    delete myFictiveConstraint;
    myFictiveConstraint = 0;
  }
}
