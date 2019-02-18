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

#ifndef ModelAPI_AttributeValidator_H
#define ModelAPI_AttributeValidator_H

#include <ModelAPI.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Validator.h>

/**
 * Generic validator for any attribute of a feature.
 */
class ModelAPI_AttributeValidator : public ModelAPI_Validator
{
public:
  //! \returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError the error string message if validation fails
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const = 0;

  MODELAPI_EXPORT ~ModelAPI_AttributeValidator();
};

#endif
