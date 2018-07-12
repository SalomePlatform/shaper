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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef BuildAPI_Polyline_H_
#define BuildAPI_Polyline_H_

#include "BuildAPI.h"

#include <BuildPlugin_Polyline.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Polyline
/// \ingroup CPPHighAPI
/// \brief Interface for Polyline feature.
class BuildAPI_Polyline: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
    explicit BuildAPI_Polyline(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Polyline(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                             const bool theIsClosed);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Polyline();

  INTERFACE_2(BuildPlugin_Polyline::ID(),
              baseObjects, BuildPlugin_Polyline::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */,
              closed, BuildPlugin_Polyline::CLOSED_ID(),
              ModelAPI_AttributeBoolean, /** Closed flag */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Set closed flag
  BUILDAPI_EXPORT void setClosed(const bool theIsClosed);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Polyline object.
typedef std::shared_ptr<BuildAPI_Polyline> PolylinePtr;

/// \ingroup CPPHighAPI
/// \brief Create Polyline feature.
BUILDAPI_EXPORT
PolylinePtr addPolyline3D(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const bool theIsClosed = false);

#endif // BuildAPI_Polyline_H_
