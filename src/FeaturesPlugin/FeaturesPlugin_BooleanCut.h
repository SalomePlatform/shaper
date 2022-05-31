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

#ifndef FeaturesPlugin_BooleanCut_H_
#define FeaturesPlugin_BooleanCut_H_

#include "FeaturesPlugin_Boolean.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>

/// \class FeaturesPlugin_BooleanCut
/// \ingroup Plugins
/// \brief Feature for applying of Boolean Cut operation.
class FeaturesPlugin_BooleanCut: public FeaturesPlugin_Boolean
{
public:

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Cut");
    return MY_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_BooleanCut::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

public:

  /// Use plugin manager for features creation.
  FeaturesPlugin_BooleanCut();

};

#endif
