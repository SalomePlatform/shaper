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

#ifndef FeaturesPlugin_Union_H_
#define FeaturesPlugin_Union_H_

#include "FeaturesPlugin_VersionedBoolean.h"

/// \class FeaturesPlugin_Union
/// \ingroup Plugins
/// \brief Feature for applying of Union operations on Shapes. Union removes shared shapes from
///        several shapes and combines them into one.
class FeaturesPlugin_Union : public FeaturesPlugin_VersionedBoolean
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Union");
    return MY_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
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

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Union::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Union();
};

#endif
