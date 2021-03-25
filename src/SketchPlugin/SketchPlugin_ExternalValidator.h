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

#ifndef SketchPlugin_ExternalValidator_H
#define SketchPlugin_ExternalValidator_H

#include <SketchPlugin.h>
#include <ModelAPI_AttributeValidator.h>

/**\class SketchPlugin_ExternalValidator
 * \ingroup Validators
 * \brief Validator for the points selection
 *
 * Allows to select points only.
 */
class SketchPlugin_ExternalValidator : public ModelAPI_AttributeValidator
{
public:
  /// returns true if the feature of attribute do not contain external features
  /// in the given attribute and
  /// among attributes listed in the arguments
  /// \param theAttribute an attribute to check
  /// \param theArguments a filter parameters
  /// \param theError error message
  SKETCHPLUGIN_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                           const std::list<std::string>& theArguments,
                                           Events_InfoMessage& theError) const;

protected:
  /// returns true if the feature of the attribute is external
  /// \param theAttribute an attribute to check
  bool isExternalAttribute(const AttributePtr& theAttribute) const;
};

#endif
