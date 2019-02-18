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

#ifndef GeomValidators_Intersected_H
#define GeomValidators_Intersected_H

#include <GeomValidators.h>

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Attribute.h>

/// \class GeomValidators_Intersected
/// \ingroup Validators
/// \brief Validates that selected shapes are intersected.
class GeomValidators_Intersected : public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the selection
  /// is intersected with shape given as a name of attribute.
  /// \param[in] theAttribute an attribute to check.
  /// \param[in] theArguments a filter parameters.
  /// \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const;
};

#endif
