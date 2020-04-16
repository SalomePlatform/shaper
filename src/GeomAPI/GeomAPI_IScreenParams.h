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

#ifndef GeomAPI_IScreenParams_H_
#define GeomAPI_IScreenParams_H_

#include "GeomAPI_Pln.h"

/**
* A class which defines an interface of object which has to be provided with
* current screen parameters
*/
class GeomAPI_IScreenParams
{
public:
  virtual ~GeomAPI_IScreenParams() {}

  /**
  * Set plane of active view screen
  *   \param theScreenPlane - the screen plane
  */
  virtual void setScreenPlane(GeomPlanePtr theScreenPlane) = 0;

  /**
  * Set a scale of active view
  * \param theScale a view scale
  */
  virtual void setViewScale(double theScale) = 0;
};

typedef std::shared_ptr<GeomAPI_IScreenParams> GeomScreenParamsPtr;

#endif