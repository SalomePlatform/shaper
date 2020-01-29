// Copyright (C) 2020  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_Defeaturing_H_
#define FeaturesPlugin_Defeaturing_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

class GeomAlgoAPI_MakeShape;
class GeomAPI_Shape;

/// \class FeaturesPlugin_Defeaturing
/// \ingroup Plugins
/// \brief Feature for the removal of the unwanted parts or features from the model.
class FeaturesPlugin_Defeaturing : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Defeaturing");
    return MY_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Defeaturing::ID();
    return MY_KIND;
  }

  /// Attribute name of main objects.
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }

  /// Performs the defeaturing algorithm and stores it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Defeaturing();

private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                    const std::shared_ptr<GeomAPI_Shape> theResultShape,
                    const std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape);
};

#endif
