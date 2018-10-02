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

#ifndef CollectionPlugin_Validators_H
#define CollectionPlugin_Validators_H

#include "CollectionPlugin.h"
#include <ModelAPI_FeatureValidator.h>

/**\class SketchPlugin_SolverErrorValidator
* \ingroup Validators
* \brief Validator for the solver error.
*
* Simply checks that solver error attribute is empty. Returns the attribute value as an error.
*/
class CollectionPlugin_FieldValidator : public ModelAPI_FeatureValidator
{
public:
  //! returns true if there are no solver errors
  //! \param theFeature the checked feature
  //! \param theArguments arguments of the feature (not used)
  //! \param theError error message
  virtual bool isValid(const FeaturePtr& theFeature,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const;
};

#endif