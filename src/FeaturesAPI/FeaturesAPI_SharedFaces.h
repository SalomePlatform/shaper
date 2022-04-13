// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_SharedFaces_H_
#define FeaturesAPI_SharedFaces_H_

#include "FeaturesAPI.h"

#include "FeaturesPlugin_GroupSharedFaces.h"

#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelAPI_Document;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_SharedFaces
/// \ingroup CPPHighAPI
/// \brief Interface to find shared faces
class FeaturesAPI_SharedFaces: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_SharedFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  FEATURESAPI_EXPORT
  explicit FeaturesAPI_SharedFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const ModelHighAPI_Selection& theObject,
                                   const std::string & theNameGroup);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_SharedFaces();

  INTERFACE_2(FeaturesPlugin_GroupSharedFaces::ID(),
              objectselected, FeaturesPlugin_GroupSharedFaces::OBJECT_ID(),
              ModelAPI_AttributeSelection, /** object selected*/,
              groupname, FeaturesPlugin_GroupSharedFaces::GROUP_NAME_ID(),
              ModelAPI_AttributeString, /** group name*/)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on the SharedFaces object.
typedef std::shared_ptr<FeaturesAPI_SharedFaces> SharedFacesPtr;

/// \ingroup CPPHighAPI
/// \brief Create a group of shared faces in a compsolid or a compound
/// \param thePart the part
/// \param theObject the selected object
/// \param theNameGroup the group name
FEATURESAPI_EXPORT
SharedFacesPtr getSharedFaces(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Selection& theObject,
                              const std::string & theNameGroup);

#endif // FeaturesAPI_SharedFaces_H_
