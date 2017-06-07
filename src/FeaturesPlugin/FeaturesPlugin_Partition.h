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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef FeaturesPlugin_Partition_H_
#define FeaturesPlugin_Partition_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_Partition.h>

/// \class FeaturesPlugin_Partition
/// \ingroup Plugins
/// \brief Feature for applying of Partition operations on Shapes. Partition makes conjunctional
/// faces of solids as shared. The result of partitions is a compsolid.
class FeaturesPlugin_Partition : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Partition");
    return MY_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Partition::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Partition();

private:
  /// Stores result of generation.
  void storeResult(ListOfShape& theObjects,
                   ListOfShape& thePlanes,
                   const GeomShapePtr theResultShape,
                   const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                   const int theIndex = 0);
};

#endif
