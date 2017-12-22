// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

#ifndef BuildAPI_Solid_H_
#define BuildAPI_Solid_H_

#include "BuildAPI.h"

#include <BuildPlugin_Solid.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Solid
/// \ingroup CPPHighAPI
/// \brief Interface for Solid feature.
class BuildAPI_Solid: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Solid(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Solid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Solid();

  INTERFACE_1(BuildPlugin_Solid::ID(),
              baseObjects, BuildPlugin_Solid::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Solid object.
typedef std::shared_ptr<BuildAPI_Solid> SolidPtr;

/// \ingroup CPPHighAPI
/// \brief Create Solid feature.
BUILDAPI_EXPORT
SolidPtr addSolid(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // BuildAPI_Solid_H_
