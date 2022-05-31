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

#ifndef ModelAPI_FeatureValidator_H
#define ModelAPI_FeatureValidator_H

#include "ModelAPI.h"
#include "ModelAPI_Validator.h"

/**\class ModelAPI_FeatureValidator
 * \ingroup DataModel
 * \brief The geneneric validator for the whole feature.
 *
 * Can be redefined for some specific feature, but by default for each feature this validator is
 * used: it checks each argument of the feature and if one of it is not valid (and obligatory),
 * the hole feature is invalid.
 */
class MODELAPI_EXPORT ModelAPI_FeatureValidator : public ModelAPI_Validator
{
 public:
  /// Default constructor
  ModelAPI_FeatureValidator();
  /// Virtual destructor
  virtual ~ModelAPI_FeatureValidator();

  /// Returns true if feature and/or attributes are valid
  /// \param theFeature the validated feature
  /// \param theArguments list of string, feature attribute names: dependent attributes
  /// \param theError the error string message if validation fails
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const = 0;

  /// Returns true if the attribute in feature is not obligatory for the feature execution.
  ///Returns false by default.
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
