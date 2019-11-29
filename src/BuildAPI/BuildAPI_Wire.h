// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef BuildAPI_Wire_H_
#define BuildAPI_Wire_H_

#include "BuildAPI.h"

#include <BuildPlugin_Wire.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Wire
/// \ingroup CPPHighAPI
/// \brief Interface for Wire feature.
class BuildAPI_Wire: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Wire(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Wire(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::list<ModelHighAPI_Selection>& theBaseObjects,
                         const bool theComputeIntersections = false);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Wire();

  INTERFACE_2(BuildPlugin_Wire::ID(),
              baseObjects, BuildPlugin_Wire::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */,
              computeIntersections, BuildPlugin_Wire::INTERSECT_ID(),
              ModelAPI_AttributeBoolean, /** Intersect edges */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Adds closed contour.
  BUILDAPI_EXPORT
  void addContour();

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Wire object.
typedef std::shared_ptr<BuildAPI_Wire> WirePtr;

/// \ingroup CPPHighAPI
/// \brief Create Wire feature.
BUILDAPI_EXPORT
WirePtr addWire(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const bool theComputeIntersections = false);

#endif // BuildAPI_Wire_H_
