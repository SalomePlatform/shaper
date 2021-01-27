// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_Loft_H_
#define FeaturesPlugin_Loft_H_

#include "FeaturesPlugin.h"

#include <GeomAlgoAPI_Loft.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>

class GeomAPI_Pnt;

/// \class FeaturesPlugin_Loft
/// \ingroup Plugins
/// \brief Feature for creation of extrusion along a path.
/// Pipe creates extrusion of objects along a path.
/// It produces the following results from objects:
/// Vertex -> Edge
/// Edge -> Face
/// Wire -> Shell
/// Face -> Solid
class FeaturesPlugin_Loft : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_FEATURE_ID("Loft");
    return MY_FEATURE_ID;
  }

  /// Attribute name for first object selected.
  inline static const std::string& FIRST_OBJECT_ID()
  {
    static const std::string MY_FIRST_OBJECT("first_object");
    return MY_FIRST_OBJECT;
  }

  /// Attribute name for second object selected.
  inline static const std::string& SECOND_OBJECT_ID()
  {
    static const std::string MY_SECOND_OBJECT("second_object");
    return MY_SECOND_OBJECT;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Loft::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Loft();
};

#endif
