// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_Intersection_H_
#define FeaturesPlugin_Intersection_H_

#include "FeaturesPlugin_VersionedBoolean.h"

class GeomAlgoAPI_MakeShape;

/// \class FeaturesPlugin_Intersection
/// \ingroup Plugins
/// \brief Intersection feature takes a list of shapes as objects and list of shapes as tools.
/// The types of objects and tools may be different:
/// whole objects, compsoilds, solids, shells, faces or edges.
/// The result is less than the minimal dimension from pair of intersection:
/// for two solids or two faces it is wire, for the edge and face it is vertex, etc.
class FeaturesPlugin_Intersection : public FeaturesPlugin_VersionedBoolean
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Intersection");
    return MY_ID;
  }

  /// Attribute name of objects.
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }

  /// Attribute name of use fuzzy parameter.
  inline static const std::string& USE_FUZZY_ID()
  {
    static const std::string MY_USE_FUZZY_ID("use_fuzzy");
    return MY_USE_FUZZY_ID;
  }

  /// Attribute name of fuzzy parameter.
  inline static const std::string& FUZZY_PARAM_ID()
  {
    static const std::string MY_FUZZY_PARAM_ID("fuzzy_param");
    return MY_FUZZY_PARAM_ID;
  }

  /// Returns the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Intersection::ID();
    return MY_KIND;
  }

  /// Executes feature.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Intersection();
};

#endif
