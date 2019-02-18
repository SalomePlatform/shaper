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

#include <PlaneGCSSolver_Solver.h>
#include <Events_LongOp.h>

// Multiplier to correlate IDs of SketchPlugin constraint and primitive PlaneGCS constraints
static const int THE_CONSTRAINT_MULT = 10;


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

void PlaneGCSSolver_Solver::addConstraint(const ConstraintID& theMultiConstraintID,
                                          const std::list<GCSConstraintPtr>& theConstraints)
{
  int anID = theMultiConstraintID > CID_UNKNOWN ?
             theMultiConstraintID * THE_CONSTRAINT_MULT :
             theMultiConstraintID;

  for (std::list<GCSConstraintPtr>::const_iterator anIt = theConstraints.begin();
       anIt != theConstraints.end(); ++anIt) {
    GCSConstraintPtr aConstraint = *anIt;
    aConstraint->setTag(anID);
    myEquationSystem->addConstraint(aConstraint.get());
    myConstraints[theMultiConstraintID].insert(aConstraint);

    if (anID > CID_UNKNOWN)
      ++anID;
  }

  if (theMultiConstraintID >= CID_UNKNOWN)
    myDOF = -1;
  myInitilized = false;
}

void PlaneGCSSolver_Solver::removeConstraint(const ConstraintID& theID)
{
  ConstraintMap::iterator aFound = myConstraints.find(theID);
  if (aFound != myConstraints.end()) {
    for (std::set<GCSConstraintPtr>::iterator anIt = aFound->second.begin();
         anIt != aFound->second.end(); ++anIt)
      myEquationSystem->clearByTag((*anIt)->getTag());

    myConstraints.erase(aFound);
  }

  if (myConstraints.empty()) {
    myEquationSystem->clear();
    myDOF = (int)myParameters.size();
  } else if (theID >= CID_UNKNOWN)
    myDOF = -1;

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

  if (aResult == GCS::Failed) {
    // DogLeg solver failed without conflicting constraints, try to use Levenberg-Marquardt solver
    diagnose(GCS::LevenbergMarquardt);
    aResult = (GCS::SolveStatus)myEquationSystem->solve(myParameters, true,
                                                        GCS::LevenbergMarquardt);
    if (aResult == GCS::Failed) {
      diagnose(GCS::BFGS);
      aResult = (GCS::SolveStatus)myEquationSystem->solve(myParameters, true, GCS::BFGS);
    }
  }
  Events_LongOp::end(this);

  // collect information about conflicting constraints every time,
  // sometimes solver reports about succeeded recalculation but has conflicting constraints
  // (for example, apply horizontal constraint for a copied feature)
  collectConflicting();
  if (!myConflictingIDs.empty())
    aResult = GCS::Failed;

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

void PlaneGCSSolver_Solver::collectConflicting(bool withRedundant)
{
  GCS::VEC_I aConflict;
  myEquationSystem->getConflicting(aConflict);
  // convert PlaneGCS constraint IDs to SketchPlugin's ID
  for (GCS::VEC_I::const_iterator anIt = aConflict.begin(); anIt != aConflict.end(); ++anIt)
    myConflictingIDs.insert((*anIt) / THE_CONSTRAINT_MULT);

  if (withRedundant) {
    myEquationSystem->getRedundant(aConflict);
    // convert PlaneGCS constraint IDs to SketchPlugin's ID
    for (GCS::VEC_I::const_iterator anIt = aConflict.begin(); anIt != aConflict.end(); ++anIt)
      myConflictingIDs.insert((*anIt) / THE_CONSTRAINT_MULT);
  }

  myConfCollected = true;
}

int PlaneGCSSolver_Solver::dof()
{
  if (myDOF < 0 && !myConstraints.empty())
    diagnose();
  return myDOF;
}

void PlaneGCSSolver_Solver::diagnose(const GCS::Algorithm& theAlgo)
{
  myEquationSystem->declareUnknowns(myParameters);
  myDOF = myEquationSystem->diagnose(theAlgo);
  myDiagnoseBeforeSolve = false;
}

void PlaneGCSSolver_Solver::addFictiveConstraintIfNecessary()
{
  if (!myConstraints.empty() &&
      myConstraints.find(CID_MOVEMENT) == myConstraints.end())
    return;

  if (myFictiveConstraint)
    return; // no need several fictive constraints

  int aDOF = myDOF;
  double* aParam = createParameter();
  double* aFictiveParameter = new double(0.0);

  myFictiveConstraint = new GCS::ConstraintEqual(aFictiveParameter, aParam);
  myFictiveConstraint->setTag(CID_FICTIVE);
  myEquationSystem->addConstraint(myFictiveConstraint);
  // DoF should not be changed when adding fictive constraint
  myDOF = aDOF;
}

void PlaneGCSSolver_Solver::removeFictiveConstraint()
{
  if (myFictiveConstraint) {
    myEquationSystem->removeConstraint(myFictiveConstraint);
    myParameters.pop_back();

    GCS::VEC_pD aParams = myFictiveConstraint->params();
    for (GCS::VEC_pD::iterator anIt = aParams.begin(); anIt != aParams.end(); ++ anIt)
      delete *anIt;
    delete myFictiveConstraint;
    myFictiveConstraint = 0;
  }
}
