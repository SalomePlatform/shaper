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

#ifndef PlaneGCSSolver_Update_H_
#define PlaneGCSSolver_Update_H_

#include <ModelAPI_Feature.h>

#include <list>
#include <memory>
#include <string>

class SketchSolver_Constraint;
class PlaneGCSSolver_Update;
typedef std::shared_ptr<PlaneGCSSolver_Update> UpdaterPtr;

/** \class   PlaneGCSSolver_Update
 *  \ingroup Plugins
 *  \brief   Send events to listeners about changing a feature
 */
class PlaneGCSSolver_Update
{
public:
  PlaneGCSSolver_Update(UpdaterPtr theNext = UpdaterPtr())
    : myNext(theNext)
  {}

  virtual ~PlaneGCSSolver_Update() {}

  /// \brief Attach listener
  /// \param theObserver [in]  object which want to receive notifications
  /// \param theType     [in]  receive notifications about changing objects
  ///                          of theType and their derivatives
  virtual void attach(SketchSolver_Constraint* theObserver, const std::string& theType) = 0;

  /// \brief Detach listener
  void detach(SketchSolver_Constraint* theObserver) {
    std::list<SketchSolver_Constraint*>::iterator anIt = myObservers.begin();
    for (; anIt != myObservers.end(); ++anIt)
      if (*anIt == theObserver) {
        myObservers.erase(anIt);
        break;
      }
    // detach listener from all senders
    if (myNext)
      myNext->detach(theObserver);
  }

  /// \brief Send notification about update of the feature to all interested
  virtual void update(const FeaturePtr& theFeature) = 0;

protected:
  UpdaterPtr myNext; ///< next updater, access if current one unable to process request
  std::list<SketchSolver_Constraint*> myObservers; ///< list of observers
};

#endif
