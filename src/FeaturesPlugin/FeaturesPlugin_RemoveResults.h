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

#ifndef FeaturesPlugin_RemoveResults_H_
#define FeaturesPlugin_RemoveResults_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_RemoveResults
/// \ingroup Plugins
/// \brief AN internal, hidden feature for results removal.
class FeaturesPlugin_RemoveResults : public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_RemoveResults();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("RemoveResults");
    return MY_ID;
  }

  /// Attribute name of removed shapes selector.
  inline static const std::string& RESULTS_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("results");
    return MY_BASE_SHAPE_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_RemoveResults::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Does nothing, has no results.
  FEATURESPLUGIN_EXPORT virtual void execute() {}

  /// Hidden from the user.
  FEATURESPLUGIN_EXPORT virtual bool isInHistory() {return false;}
};

#endif
