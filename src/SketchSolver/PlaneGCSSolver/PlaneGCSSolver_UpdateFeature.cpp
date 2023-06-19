// Copyright (C) 2014-2023  CEA, EDF
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

#include <PlaneGCSSolver_UpdateFeature.h>
#include <SketchSolver_Constraint.h>

void PlaneGCSSolver_UpdateFeature::attach(SketchSolver_Constraint* theObserver,
                                          const std::string& theType)
{
  if (theType == GROUP())
    myObservers.push_back(theObserver);
  else
    myNext->attach(theObserver, theType);
}

void PlaneGCSSolver_UpdateFeature::update(const FeaturePtr& theFeature)
{
  std::list<SketchSolver_Constraint*>::iterator anIt = myObservers.begin();
  for (; anIt != myObservers.end(); ++anIt)
    (*anIt)->notify(theFeature, this);
}
