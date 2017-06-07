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

#ifndef ModelAPI_ResultCompSolid_H_
#define ModelAPI_ResultCompSolid_H_

#include "ModelAPI_Result.h"
#include "ModelAPI_ResultBody.h"
#include <string>

/**\class ModelAPI_ResultCompSolid
* \ingroup DataModel
* \brief The comp solid (container of results) result of a feature.
*
* Provides a conainer of body result that may be displayed in the viewer.
*/
class ModelAPI_ResultCompSolid : public ModelAPI_ResultBody
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_ResultCompSolid();
  /// Returns the group identifier of this result

  /// Returns the number of sub-elements
  virtual int numberOfSubs(bool forTree = false) const = 0;

  /// Returns the sub-result by zero-base index
  virtual std::shared_ptr<ModelAPI_ResultBody> subResult(const int theIndex,
                                                         bool forTree = false) const = 0;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  virtual bool isSub(ObjectPtr theObject) const = 0;

  /// Set displayed flag to the result and all sub results
  /// \param theDisplay a boolean value
  MODELAPI_EXPORT virtual void setDisplayed(const bool theDisplay);

protected:
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultCompSolid> ResultCompSolidPtr;

#endif
