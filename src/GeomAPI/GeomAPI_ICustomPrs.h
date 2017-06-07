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

#ifndef GeomAPI_ICustomPrs_H
#define GeomAPI_ICustomPrs_H

#include "GeomAPI.h"
#include "GeomAPI_AISObject.h"
#include "GeomAPI_AISObject.h"

#include <vector>

class ModelAPI_Result;

/** \class GeomAPI_ICustomPrs
 *  \ingroup DataModel
 *  \brief Interface of a class which can provide specific customization of
 *         object presentation
 */
class GeomAPI_ICustomPrs
{
public:
  GEOMAPI_EXPORT virtual ~GeomAPI_ICustomPrs();

  /// Modifies the given presentation in the custom way.
  virtual bool customisePresentation(std::shared_ptr<ModelAPI_Result> theResult,
                                     AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs) = 0;
};

typedef std::shared_ptr<GeomAPI_ICustomPrs> GeomCustomPrsPtr;

#endif
