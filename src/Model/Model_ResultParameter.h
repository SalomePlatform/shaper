// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef MODEL_RESULTPARAMETER_H_
#define MODEL_RESULTPARAMETER_H_

#include "Model.h"
#include <Model_Document.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultParameter.h>

/**\class Model_ResultParameter
 * \ingroup DataModel
 * \brief Result of parameter expression. Contains only double value computed result attribute.
 */
class Model_ResultParameter : public ModelAPI_ResultParameter
{
 public:
  MODEL_EXPORT virtual ~Model_ResultParameter();
  MODEL_EXPORT virtual void initAttributes();

 protected:
  Model_ResultParameter();

  friend class Model_Objects;
};

#endif
