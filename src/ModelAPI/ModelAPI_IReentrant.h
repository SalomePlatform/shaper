// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef ModelAPI_IReentrant_H
#define ModelAPI_IReentrant_H

#include "ModelAPI.h"

#include <vector>
#include <memory>
#include <string>

class Events_Message;

/** \class ModelAPI_IReentrant
 *  \ingroup DataModel
 *  \brief Interface of a class which can process specific messages
 */
class ModelAPI_IReentrant
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_IReentrant();

  /// Apply information of the message to current object.
  /// \param theMessage a container of information
  /// \return a next active attribute name
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage) = 0;
};

typedef std::shared_ptr<ModelAPI_IReentrant> ModelReentrantPtr;

#endif
