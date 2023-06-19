// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef FeaturesPlugin_Boolean_H_
#define FeaturesPlugin_Boolean_H_

#include "FeaturesPlugin_VersionedBoolean.h"

/// \class FeaturesPlugin_Boolean
/// \ingroup Plugins
/// \brief Feature for applying of Boolean operations on Solids.
/// Supports four kinds of Boolean operations: Cut, Fuse, Common and Smash.
class FeaturesPlugin_Boolean : public FeaturesPlugin_VersionedBoolean
{
public:
  enum OperationType {
    BOOL_CUT,
    BOOL_FUSE,
    BOOL_COMMON,
    BOOL_FILL,
    BOOL_SMASH
  };

  /// Attribute name of main objects.
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }

  /// Attribute name of tool objects.
  inline static const std::string& TOOL_LIST_ID()
  {
    static const std::string MY_TOOL_LIST_ID("tool_objects");
    return MY_TOOL_LIST_ID;
  }

  /// Attribute name of use fuzzy parameter.
  inline static const std::string& USE_FUZZY_ID()
  {
    static const std::string MY_USE_FUZZY_ID("use_fuzzy");
    return MY_USE_FUZZY_ID;
  }

  /// Attribute name of fuzzy parameter.
  inline static const std::string& FUZZY_PARAM_ID()
  {
    static const std::string MY_FUZZY_PARAM_ID("fuzzy_param");
    return MY_FUZZY_PARAM_ID;
  }

  /// \return boolean operation type.
  FEATURESPLUGIN_EXPORT OperationType operationType();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

protected:

  /// Use plugin manager for features creation.
  FeaturesPlugin_Boolean(const OperationType theOperationType);

  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                    const ListOfShape& theTools,
                    const std::shared_ptr<GeomAPI_Shape> theResultShape,
                    const GeomMakeShapePtr& theMakeShape);

  /// Store result shape if it is not empty and increase results counter
  void storeResult(const ListOfShape& theObjects,
                   const ListOfShape& theTools,
                   const GeomShapePtr theResultShape,
                   int& theResultIndex,
                   std::shared_ptr<GeomAlgoAPI_MakeShapeList> theMakeShapeList,
                   std::vector<ModelAPI_Tools::ResultBaseAlgo>& theResultBaseAlgoList);

private:
  OperationType myOperationType;
};

#endif
