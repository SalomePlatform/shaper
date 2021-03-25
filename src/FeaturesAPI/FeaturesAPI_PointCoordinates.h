// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_PointCoordinates_H_
#define FeaturesAPI_PointCoordinates_H_

#include "FeaturesAPI.h"

#include <list>
#include <memory>

class ModelAPI_Document;
class ModelHighAPI_Selection;

/// \ingroup CPPHighAPI
/// \brief  Get the point coordinates.
FEATURESAPI_EXPORT
std::list<double> getPointCoordinates(const std::shared_ptr<ModelAPI_Document>& thePart,
                                      const ModelHighAPI_Selection& thePoint);

#endif // FeaturesAPI_PointCoordinates_H_
