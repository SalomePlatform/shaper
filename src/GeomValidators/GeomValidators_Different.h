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

#ifndef GeomValidators_Different_H
#define GeomValidators_Different_H

#include <GeomValidators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

/** \class GeomValidators_Different
 *  \ingroup Validators
 *  \brief Validates that attributes are not the same.
 */
class GeomValidators_Different : public ModelAPI_FeatureValidator
{
public:
  /** \brief Returns true if feature and/or attributes are valid.
   *  \param[in] theFeature the validated feature.
   *  \param[in] theArguments the arguments in the configuration file for this validator.
   *  \param[out] theError error message.
   *  \returns true if feature is valid.
   */
  GEOMVALIDATORS_EXPORT virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const;

  GEOMVALIDATORS_EXPORT virtual
    bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
