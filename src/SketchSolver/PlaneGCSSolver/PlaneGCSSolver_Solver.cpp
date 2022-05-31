// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
static const int THE_CONSTRAINT_MULT = 100;


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

    if (anID > CID_UNKNOWN)
      ++anID;
  }
  myConstraints[theMultiConstraintID] = theConstraints;

  if (theMultiConstraintID >= CID_UNKNOWN)
    myDOF = -1;
  myInitilized = false;
}

void PlaneGCSSolver_Solver::removeConstraint(const ConstraintID& theID)
{
  ConstraintMap::iterator aFound = myConstraints.find(theID);
  if (aFound != myConstraints.end()) {
    for (std::list<GCSConstraintPtr>::iterator anIt = aFound->second.begin();
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

void PlaneGCSSolver_Solver::getFreeParameters(GCS::SET_pD& theFreeParams)
{
  if (myConstraints.empty())
    theFreeParams.insert(myParameters.begin(), myParameters.end());
  else {
    GCS::VEC_pD aParametersCopy = myParameters;
    ConstraintMap aConstraintCopy = myConstraints;

    // clear the set of equations
    clear();
    // reset constraints
    myParameters = aParametersCopy;
    for (ConstraintMap::iterator anIt = aConstraintCopy.begin();
         anIt != aConstraintCopy.end(); ++anIt)
      addConstraint(anIt->first, anIt->second);

    // parameters detection works for Dense QR only
    GCS::QRAlgorithm aQRAlgo = myEquationSystem->qrAlgorithm;
    myEquationSystem->qrAlgorithm = GCS::EigenDenseQR;
    diagnose();
    GCS::VEC_pD aFreeParams;
    myEquationSystem->getDependentParams(aFreeParams);
    theFreeParams.insert(aFreeParams.begin(), aFreeParams.end());
    // revert QR decomposition algorithm
    myEquationSystem->qrAlgorithm = aQRAlgo;
  }

  if (theFreeParams.empty())
    return;

  // find all equal parameters too
  struct EqualParameters
  {
    typedef std::map<double*, std::list<GCS::SET_pD>::iterator> MapParamGroup;

    std::list<GCS::SET_pD> myEqualParams;
    MapParamGroup myGroups;

    void add(double* theParam1, double* theParam2)
    {
      MapParamGroup::iterator aFound1 = myGroups.find(theParam1);
      MapParamGroup::iterator aFound2 = myGroups.find(theParam2);

      if (aFound1 == myGroups.end()) {
        if (aFound2 == myGroups.end()) {
          // create new group
          myEqualParams.push_back(GCS::SET_pD());
          std::list<GCS::SET_pD>::iterator aGroup = --myEqualParams.end();
          aGroup->insert(theParam1);
          aGroup->insert(theParam2);
          myGroups[theParam1] = aGroup;
          myGroups[theParam2] = aGroup;
        }
        else {
          // add first parameter to the second group
          aFound2->second->insert(theParam1);
          myGroups[theParam1] = aFound2->second;
        }
      }
      else {
        if (aFound2 == myGroups.end()) {
          // add second parameter to the first group
          aFound1->second->insert(theParam2);
          myGroups[theParam2] = aFound1->second;
        }
        else if (aFound1 != aFound2) {
          // merge two groups
          GCS::SET_pD aCopy = *(aFound2->second);
          myEqualParams.erase(aFound2->second);
          for (GCS::SET_pD::iterator anIt = aCopy.begin(); anIt != aCopy.end(); ++anIt)
            myGroups[*anIt] = aFound1->second;
          aFound1->second->insert(aCopy.begin(), aCopy.end());
        }
      }
    }
  } anEqualParams;

  for (ConstraintMap::iterator anIt = myConstraints.begin(); anIt != myConstraints.end(); ++anIt)
    for (std::list<GCSConstraintPtr>::iterator aCIt = anIt->second.begin();
         aCIt != anIt->second.end(); ++aCIt) {
      if ((*aCIt)->getTypeId() == GCS::Equal)
        anEqualParams.add((*aCIt)->params()[0], (*aCIt)->params()[1]);
    }

  GCS::SET_pD aFreeParamsCopy = theFreeParams;
  for (GCS::SET_pD::iterator anIt = aFreeParamsCopy.begin();
       anIt != aFreeParamsCopy.end(); ++anIt) {
    EqualParameters::MapParamGroup::iterator aFound = anEqualParams.myGroups.find(*anIt);
    if (aFound != anEqualParams.myGroups.end())
      theFreeParams.insert(aFound->second->begin(), aFound->second->end());
  }
}

void PlaneGCSSolver_Solver::addFictiveConstraintIfNecessary()
{
  bool hasOnlyMovement = true;
  for (ConstraintMap::iterator anIt = myConstraints.begin();
       anIt != myConstraints.end() && hasOnlyMovement; ++anIt)
    hasOnlyMovement = anIt->first == CID_MOVEMENT;
  if (!hasOnlyMovement)
    return; // regular constraints are available too

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
    myEquationSystem->clearByTag(myFictiveConstraint->getTag());
    myParameters.pop_back();

    GCS::VEC_pD aParams = myFictiveConstraint->params();
    for (GCS::VEC_pD::iterator anIt = aParams.begin(); anIt != aParams.end(); ++anIt) {
      double* aPar = *anIt;
      delete aPar;
    }
    delete myFictiveConstraint;
    myFictiveConstraint = 0;
  }
}
