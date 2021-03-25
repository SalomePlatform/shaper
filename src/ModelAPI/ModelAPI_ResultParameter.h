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

#ifndef MODELAPI_RESULTPARAMETER_H_
#define MODELAPI_RESULTPARAMETER_H_

#include <ModelAPI.h>
#include <ModelAPI_Result.h>

/**\class ModelAPI_ResultParameter
 * \ingroup DataModel
 * \brief The construction element result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * Intermediate, light result that in many cases produces a result on the fly.
 */
class ModelAPI_ResultParameter : public ModelAPI_Result
{
 public:
  /// Returns the group identifier of this result
  virtual std::string groupName()
  {
    return group();
  }

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Parameters";
    return MY_GROUP;
  }
  /// The computed value attribute Id
  inline static const std::string& VALUE()
  {
    static const std::string MY_VALUE_ID("Value");
    return MY_VALUE_ID;
  }

  /// The computational error attribute Id
  inline static const std::string& STATE()
  {
    static const std::string MY_VALUE_ID("State");
    return MY_VALUE_ID;
  }

  /// The generic initialization of attributes
  virtual void initAttributes() = 0;

  /// Destructor
  MODELAPI_EXPORT ~ModelAPI_ResultParameter();

};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultParameter> ResultParameterPtr;

#endif
