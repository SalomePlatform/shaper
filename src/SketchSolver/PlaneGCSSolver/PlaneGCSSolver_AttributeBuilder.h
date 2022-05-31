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

#ifndef PlaneGCSSolver_AttributeBuilder_H_
#define PlaneGCSSolver_AttributeBuilder_H_

#include <PlaneGCSSolver_EntityBuilder.h>

/** \class   PlaneGCSSolver_AttributeBuilder
 *  \ingroup Plugins
 *  \brief   Converts ModelAPI_Attribute to the entity applicable for PlaneGCS solver
 */
class PlaneGCSSolver_AttributeBuilder : public PlaneGCSSolver_EntityBuilder
{
public:
  PlaneGCSSolver_AttributeBuilder(PlaneGCSSolver_Storage* theStorage = 0);
  PlaneGCSSolver_AttributeBuilder(const StoragePtr& theStorage);

  /// \brief Converts an attribute to the solver's entity.
  ///        Double attributes and 2D points are supported only.
  /// \param theAttribute [in]  attribute to create
  /// \return Created wrapper of the attribute applicable for specific solver
  virtual EntityWrapperPtr createAttribute(AttributePtr theAttribute);

  /// \brief Update entity by the attribute values.
  /// \return \c true if any value is updated.
  virtual bool updateAttribute(AttributePtr theAttribute, EntityWrapperPtr theEntity);

  /// \brief Blank. To be defined in derived class.
  virtual EntityWrapperPtr createFeature(FeaturePtr)
  { return EntityWrapperPtr(); }
};

#endif
