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

#ifndef PlaneGCSSolver_EntityBuilder_H_
#define PlaneGCSSolver_EntityBuilder_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_Storage.h>

/** \class   PlaneGCSSolver_EntityBuilder
 *  \ingroup Plugins
 *  \brief   API to convert data model entity to the entity applicable for PlaneGCS solver
 */
class PlaneGCSSolver_EntityBuilder
{
public:
  /// \brief Create entity in the given storage.
  ///        If the storage is empty, the entity should not be changed
  ///         while constraint solving. So, it is created out of the storage.
  PlaneGCSSolver_EntityBuilder(PlaneGCSSolver_Storage* theStorage = 0)
    : myStorage(theStorage)
  {}

  virtual ~PlaneGCSSolver_EntityBuilder() {}

  /// \brief Converts an attribute to the solver's entity.
  ///        Double attributes and 2D points are supported only.
  /// \param theAttribute [in]  attribute to create
  /// \return Created wrapper of the attribute applicable for specific solver
  virtual EntityWrapperPtr createAttribute(AttributePtr theAttribute) = 0;

  /// \brief Converts SketchPlugin's feature to the solver's entity.
  ///        Result if based on the list of already converted attributes.
  /// \param theFeature [in]  feature to create
  virtual EntityWrapperPtr createFeature(FeaturePtr) = 0;

protected:
  PlaneGCSSolver_Storage* myStorage;
};

#endif
