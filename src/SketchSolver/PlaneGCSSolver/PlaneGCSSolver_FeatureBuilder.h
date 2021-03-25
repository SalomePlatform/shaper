// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef PlaneGCSSolver_FeatureBuilder_H_
#define PlaneGCSSolver_FeatureBuilder_H_

#include <PlaneGCSSolver_AttributeBuilder.h>

typedef std::map<AttributePtr, EntityWrapperPtr> AttributeEntityMap;

/** \class   PlaneGCSSolver_FeatureBuilder
 *  \ingroup Plugins
 *  \brief   Converts SketchPlugin_Feature to the entity applicable for PlaneGCS solver
 */
class PlaneGCSSolver_FeatureBuilder : public PlaneGCSSolver_AttributeBuilder
{
public:
  PlaneGCSSolver_FeatureBuilder(PlaneGCSSolver_Storage* theStorage = 0);
  PlaneGCSSolver_FeatureBuilder(const StoragePtr& theStorage);

  /// \brief Converts an attribute to the solver's entity and stores it for further processing.
  ///        Double attributes and 2D points are supported only.
  /// \param theAttribute [in]  attribute to create
  /// \return Created wrapper of the attribute applicable for specific solver
  virtual EntityWrapperPtr createAttribute(AttributePtr theAttribute);

  /// \brief Converts SketchPlugin's feature to the solver's entity.
  ///        Result if based on the list of already converted attributes.
  /// \param theFeature [in]  feature to create
  virtual EntityWrapperPtr createFeature(FeaturePtr theFeature);

private:
  /// list of converted attributes (will be cleared when the feature is created)
  AttributeEntityMap myAttributes;
};

#endif
