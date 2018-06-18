// Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_Measurement_H_
#define FeaturesAPI_Measurement_H_

#include "FeaturesAPI.h"

#include <list>
#include <memory>

class ModelAPI_Document;
class ModelHighAPI_Selection;

/// \ingroup CPPHighAPI
/// \brief Calculate length of the edge.
FEATURESAPI_EXPORT
double measureLength(const std::shared_ptr<ModelAPI_Document>& thePart,
                     const ModelHighAPI_Selection& theEdge);

/// \ingroup CPPHighAPI
/// \brief Calculate distance between objects.
FEATURESAPI_EXPORT
double measureDistance(const std::shared_ptr<ModelAPI_Document>& thePart,
                       const ModelHighAPI_Selection& theFrom,
                       const ModelHighAPI_Selection& theTo);

/// \ingroup CPPHighAPI
/// \brief Calculate radius of circular.
FEATURESAPI_EXPORT
double measureRadius(const std::shared_ptr<ModelAPI_Document>& thePart,
                     const ModelHighAPI_Selection& theObject);

/// \ingroup CPPHighAPI
/// \brief Calculate angle(s) between edges.
FEATURESAPI_EXPORT
std::list<double> measureAngle(const std::shared_ptr<ModelAPI_Document>& thePart,
                               const ModelHighAPI_Selection& theFrom,
                               const ModelHighAPI_Selection& theTo);

#endif // FeaturesAPI_Measurement_H_
