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

#ifndef PlaneGCSSolver_UpdateFeature_H_
#define PlaneGCSSolver_UpdateFeature_H_

#include <PlaneGCSSolver_Update.h>

/** \class   PlaneGCSSolver_UpdateFeature
 *  \ingroup Plugins
 *  \brief   Send events to listeners about changing a feature
 */
class PlaneGCSSolver_UpdateFeature : public PlaneGCSSolver_Update
{
public:
  PlaneGCSSolver_UpdateFeature(UpdaterPtr theNext = UpdaterPtr())
    : PlaneGCSSolver_Update(theNext)
  {}

  /// \brief Group of entities, processed by this kind of updater
  static const std::string& GROUP()
  {
    static const std::string TYPE("Feature");
    return TYPE;
  }

  /// \brief Attach listener
  /// \param theObserver [in]  object which want to receive notifications
  /// \param theType     [in]  receive notifications about changing objects
  ///                          of theType and their derivatives
  virtual void attach(SketchSolver_Constraint* theObserver, const std::string& theType);

  /// \brief Send notification about update of the feature to all interested
  virtual void update(const FeaturePtr& theFeature);
};

#endif
