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

#ifndef GeomAPI_IPresentable_H_
#define GeomAPI_IPresentable_H_

#include "GeomAPI_AISObject.h"

/**
 * A class which defines an interface of object which is able to create its own presentation
 */
class GeomAPI_IPresentable
{
 public:
  GEOMAPI_EXPORT virtual ~GeomAPI_IPresentable();
  /** Returns the AIS preview
   *   \param thePrevious - defines a presentation if it was created previously
   */
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious) = 0;
};

typedef std::shared_ptr<GeomAPI_IPresentable> GeomPresentablePtr;

#endif
