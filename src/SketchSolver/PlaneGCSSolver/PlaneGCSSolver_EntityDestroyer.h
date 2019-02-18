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

#ifndef PlaneGCSSolver_EntityDestroyer_H_
#define PlaneGCSSolver_EntityDestroyer_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_Storage.h>

/** \class   PlaneGCSSolver_EntityDestroyer
 *  \ingroup Plugins
 *  \brief   Collect entities of PlaneGCS solver which should be removed
 */
class PlaneGCSSolver_EntityDestroyer
{
public:
  PlaneGCSSolver_EntityDestroyer() {}

  /// \brief Add entity to remove. Its parameters are stored for further remove from the storage.
  void remove(const EntityWrapperPtr& theEntity);

  /// \brief Return parameters to remove
  const GCS::SET_pD& parametersToRemove()
  { return myParams; }

private:
  GCS::SET_pD myParams; ///< set of parameters to be removed
  GCS::SET_pD myParamsOutOfStorage; ///< set of parameters not kept in the storage
};

#endif
