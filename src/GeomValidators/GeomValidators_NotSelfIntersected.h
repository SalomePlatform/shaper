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

#ifndef GeomValidators_NotSelfIntersected_H
#define GeomValidators_NotSelfIntersected_H

#include <GeomValidators.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

/// \class GeomValidators_NotSelfIntersected
/// \ingroup Validators
/// \brief Validates that selected shapes are not self intersected.
class GeomValidators_NotSelfIntersected: public ModelAPI_FeatureValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the selection
  /// is not self intersected.
  /// \param[in] theAttribute an attribute to check.
  /// \param[in] theArguments a filter parameters.
  /// \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const;

  /// \return true if the attribute in feature is not obligatory for the feature execution.
  GEOMVALIDATORS_EXPORT virtual bool isNotObligatory(std::string theFeature,
                                                     std::string theAttribute);
};

#endif
