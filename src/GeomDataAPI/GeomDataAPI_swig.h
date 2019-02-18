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

#ifndef SRC_GEOMDATAAPI_GEOMDATAAPI_SWIG_H_
#define SRC_GEOMDATAAPI_GEOMDATAAPI_SWIG_H_

  #include <GeomAPI_swig.h>
  #include <ModelAPI_swig.h>

  #include "GeomDataAPI.h"
  #include "GeomDataAPI_Point.h"
  #include "GeomDataAPI_Dir.h"
  #include "GeomDataAPI_Point2D.h"

  #include <memory>
  #include <string>
  #include <list>

  template<class T> std::shared_ptr<T> castTo(std::shared_ptr<ModelAPI_Attribute> theObject)
  {
    return std::dynamic_pointer_cast<T>(theObject);
  }

#endif /* SRC_GEOMDATAAPI_GEOMDATAAPI_SWIG_H_ */
