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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Model_ResultGroup_H_
#define Model_ResultGroup_H_

#include "Model.h"
#include <ModelAPI_ResultGroup.h>

/**\class Model_ResultGroup
 * \ingroup DataModel
 * \brief The groups result.
 *
 * Provides a compound of selected elements, without storage, one the fly.
 */
class Model_ResultGroup : public ModelAPI_ResultGroup
{
  std::shared_ptr<ModelAPI_Data> myOwnerData;  ///< data of owner of this result
public:

  /// Retuns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// Returns the compound of selected entities
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultGroup() {}

protected:
  /// Makes a body on the given feature data
  Model_ResultGroup(std::shared_ptr<ModelAPI_Data> theOwnerData);

  friend class Model_Objects;
};

#endif
