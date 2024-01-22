// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef ModelAPI_Entity_H_
#define ModelAPI_Entity_H_

#include <memory>

/**\class ModelAPI_Entity
 * \ingroup DataModel
 * \brief Represents a common parent class for Objects and documents.
 * Provided in order to make possible distinguishing of objects and documents
 * by downcasting of their pointers.
 */
class ModelAPI_Entity
{
public:
  /// Empty function which is added for virtualiation of the interface
  virtual void emptyFunction() const {}
};

typedef std::shared_ptr<ModelAPI_Entity> EntityPtr;


#endif