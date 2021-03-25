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

#ifndef FILTERSPLUGIN_VALIDATORS_H_
#define FILTERSPLUGIN_VALIDATORS_H_

#include "FiltersPlugin.h"

#include <GeomValidators_ShapeType.h>

/**
 * Validates selection of "On geometry" filter to select only
 * the shapes specified by the group type.
 */
class FiltersPlugin_ShapeTypeValidator : public GeomValidators_ShapeType
{
public:
  /// \return True if the attribute is valid. It checks whether the shape is a
  /// body subshape. Does not allow select construction shapes.
  /// \param[in]  theAttribute an attribute to check
  /// \param[in]  theArguments a filter parameters
  /// \param[out] theError error message.
  FILTERS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                      const std::list<std::string>& theArguments,
                                      Events_InfoMessage& theError) const;
};

#endif