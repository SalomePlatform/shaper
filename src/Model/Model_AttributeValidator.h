// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Model_AttributeValidator_H
#define Model_AttributeValidator_H

#include "Model.h"
#include <ModelAPI_AttributeValidator.h>

/**\class Model_AttributeValidator
 * \ingroup DataModel
 * \brief The geneneric validator for the attribute.
 *
 * Can be redefined for some specific attributes, but by default for each attribute this validator is
 * used: it checks the argument and if it contains invalid expression the attribute is invalid.
 */
class Model_AttributeValidator : public ModelAPI_AttributeValidator
{
public:
  /// \param theAttribute the checked attribute
  /// \param theArguments arguments of the attribute
  /// \param theError erros message produced by validator to the user if it fails
  /// \returns true if attribute is valid
  MODEL_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                    const std::list<std::string>& theArguments,
                                    Events_InfoMessage& theError) const;
};

#endif // Model_AttributeValidator_H
