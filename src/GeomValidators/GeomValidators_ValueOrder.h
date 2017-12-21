// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

#ifndef GeomValidators_ValueOrder_H
#define GeomValidators_ValueOrder_H

#include <GeomValidators.h>
#include <ModelAPI_AttributeValidator.h>

/**
 * Validates that the integer/double attribute is greater or equal to another attribute values
 */
class GeomValidators_GreaterOrEqual : public ModelAPI_AttributeValidator
{
public:
  //! Constructor for only one instance per application: will register the validator
  GeomValidators_GreaterOrEqual();
  //! returns true if attribute is valid
  //! \param[in] theAttribute the checked attribute
  //! \param[in] theArguments arguments of the attribute
  //! \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const;

};

/**
 * Validates that the integer/double attribute is less or equal to another attribute values
 */
class GeomValidators_LessOrEqual : public ModelAPI_AttributeValidator
{
public:
  //! Constructor for only one instance per application: will register the validator
  GeomValidators_LessOrEqual();
  //! returns true if attribute is valid
  //! \param[in] theAttribute the checked attribute
  //! \param[in] theArguments arguments of the attribute
  //! \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const;

};

#endif
