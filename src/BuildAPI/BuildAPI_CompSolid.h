// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#ifndef BuildAPI_CompSolid_H_
#define BuildAPI_CompSolid_H_

#include "BuildAPI.h"

#include <BuildPlugin_CompSolid.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_CompSolid
/// \ingroup CPPHighAPI
/// \brief Interface for CompSolid feature.
class BuildAPI_CompSolid: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_CompSolid(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_CompSolid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_CompSolid();

  INTERFACE_1(BuildPlugin_CompSolid::ID(),
              baseObjects, BuildPlugin_CompSolid::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on CompSolid object.
typedef std::shared_ptr<BuildAPI_CompSolid> CompSolidPtr;

/// \ingroup CPPHighAPI
/// \brief Create CompSolid feature.
BUILDAPI_EXPORT
CompSolidPtr addCompSolid(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // BuildAPI_CompSolid_H_
