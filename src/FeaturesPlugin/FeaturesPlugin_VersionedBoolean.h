// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_VersionedBoolean_H_
#define FeaturesPlugin_VersionedBoolean_H_

#include "FeaturesPlugin.h"
#include "FeaturesPlugin_Tools.h"

#include <GeomAPI_ShapeHierarchy.h>
#include <GeomAlgoAPI_Tools.h>

#include <ModelAPI_Feature.h>

class ModelAPI_Attribute;
class ModelAPI_Result;
class GeomAlgoAPI_MakeShapeList;

/// \class FeaturesPlugin_VersionedBoolean
/// \ingroup Plugins
/// \brief Feature controls a version of Boolean operations.
class FeaturesPlugin_VersionedBoolean : public ModelAPI_Feature
{
protected:
  static const std::string& BOP_VERSION_9_4()
  {
    static const std::string VERSION("v9.4");
    return VERSION;
  }

  /// Use plugin manager for features creation.
  FeaturesPlugin_VersionedBoolean() {}

  /// Initialize version field of the Boolean feature.
  /// The version is initialized for newly created features,
  /// not read from previously stored document.
  void initVersion(const std::string& theVersion,
                   const std::shared_ptr<ModelAPI_Attribute> theObjectsAttr
                            = std::shared_ptr<ModelAPI_Attribute>(),
                   const std::shared_ptr<ModelAPI_Attribute> theToolsAttr
                            = std::shared_ptr<ModelAPI_Attribute>());

  /// Process SelectionList attribute and fill the objects hierarchy.
  bool processAttribute(const std::string& theAttributeName,
                        GeomAPI_ShapeHierarchy& theObjects,
                        ListOfShape& thePlanesList);

  /// Perform Boolean operation of the object with the tools.
  /// In case of theResultCompound is not empty, the result of Boolean operation
  /// is added to this compound, and corresponding ResultBody is not generated.
  /// \return \c false if something went wrong
  bool processObject(const GeomAlgoAPI_Tools::BOPType theBooleanType,
                     const GeomShapePtr& theObject,
                     const ListOfShape& theTools,
                     const ListOfShape& thePlanes,
                     int& theResultIndex,
                     std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
                     ListOfShape& theResultShapesList,
                     GeomShapePtr theResulCompound = GeomShapePtr());

  /// Perform Boolean operation of the Compsolid with the tools
  /// In case of theResultCompound is not empty, the result of Boolean operation
  /// is added to this compound, and corresponding ResultBody is not generated.
  /// \return \c false if something went wrong
  bool processCompsolid(const GeomAlgoAPI_Tools::BOPType theBooleanType,
                        GeomAPI_ShapeHierarchy& theCompsolidHierarchy,
                        const GeomShapePtr& theCompsolid,
                        const ListOfShape& theTools,
                        const ListOfShape& thePlanes,
                        int& theResultIndex,
                        std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
                        ListOfShape& theResultShapesList,
                        GeomShapePtr theResulCompound = GeomShapePtr());

  /// Perform Boolean operation of the Compound with the tools
  /// In case of theResultCompound is not empty, the result of Boolean operation
  /// is added to this compound, and corresponding ResultBody is not generated.
  /// \return \c false if something went wrong
  bool processCompound(const GeomAlgoAPI_Tools::BOPType theBooleanType,
                       GeomAPI_ShapeHierarchy& theCompoundHierarchy,
                       const GeomShapePtr& theCompound,
                       const ListOfShape& theTools,
                       int& theResultIndex,
                       std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
                       ListOfShape& theResultShapesList,
                       GeomShapePtr theResulCompound = GeomShapePtr());

  /// Resize planes to fit them to the bounding box of the given lins of objects.
  static void resizePlanes(const ListOfShape& theObjects,
                           ListOfShape& thePlanes,
                           std::shared_ptr<GeomAlgoAPI_MakeShapeList>& theMakeShapeList);

  /// Process unused sub-shapes of compounds.
  /// Keep the compound hierarchy, but merge top-level compounds
  /// into a single compound and add the result of the FUSE operation.
  GeomShapePtr keepUnusedSubsOfCompound(
      const GeomShapePtr& theResult,
      const GeomAPI_ShapeHierarchy& theObjectsHierarchy,
      const GeomAPI_ShapeHierarchy& theToolsHierarchy,
      std::shared_ptr<GeomAlgoAPI_MakeShapeList> theMakeShapeList);
};

#endif
