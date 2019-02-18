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

#ifndef ModelAPI_ResultGroup_H_
#define ModelAPI_ResultGroup_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>
#include <memory>
#include <string>

/**\class ModelAPI_ResultGroup
 * \ingroup DataModel
 * \brief The groups result.
 *
 * Provides a compound of selected elements, without storage, one the fly.
 */
class ModelAPI_ResultGroup : public ModelAPI_Result
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_ResultGroup();
  /// Returns the group identifier of this result
  MODELAPI_EXPORT virtual std::string groupName();

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Groups";
    return MY_GROUP;
  }

  /// default color for a result body
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_GROUP_COLOR("150,150,180");
    return RESULT_GROUP_COLOR;
  }

};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultGroup> ResultGroupPtr;

#endif
