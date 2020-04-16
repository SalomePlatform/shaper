// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_VersionedChFi_H_
#define FeaturesPlugin_VersionedChFi_H_

#include "FeaturesPlugin.h"

#include <GeomAPI_Shape.h>
#include <GeomAPI_ShapeHierarchy.h>

#include <ModelAPI_Feature.h>

class GeomAlgoAPI_MakeShape;

/// \class FeaturesPlugin_VersionedChFi
/// \ingroup Plugins
/// \brief Base feature for the Fillet and the Chamfer operations
///        which supports versioning.
class FeaturesPlugin_VersionedChFi : public ModelAPI_Feature
{
public:
  /// Performs the fillet algorithm and stores it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

protected:
  FeaturesPlugin_VersionedChFi() {}

  /// Intialize the version for the newly created feature.
  void initVersion(const std::shared_ptr<ModelAPI_Attribute>& theObjectsAttr);

  /// Process SelectionList attribute and fill the objects hierarchy.
  bool processAttribute(const std::shared_ptr<ModelAPI_Attribute>& theAttribute,
                        GeomAPI_ShapeHierarchy& theObjects);

  /// Return attribute storing the selected objects of the operation.
  virtual std::shared_ptr<ModelAPI_Attribute> objectsAttribute() = 0;

  /// Return name of modified shape prefix name
  virtual const std::string& modifiedShapePrefix() const = 0;

  /// Run chamfer/fillet operation and returns the modification algorithm if succeed.
  virtual std::shared_ptr<GeomAlgoAPI_MakeShape>
      performOperation(const GeomShapePtr& theSolid, const ListOfShape& theEdges) = 0;
};

#endif
