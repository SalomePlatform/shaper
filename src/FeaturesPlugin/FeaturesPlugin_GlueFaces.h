// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_GlueFaces_H_
#define FeaturesPlugin_GlueFaces_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>


/// \class FeaturesPlugin_GlueFaces
/// \ingroup Plugins
/// \brief Feature to glue duplicated faces.
class FeaturesPlugin_GlueFaces : public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_GlueFaces();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("GlueFaces");
    return MY_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_GlueFaces::ID();
    return MY_KIND;
  }

  /// Attribute name of referenced objects.
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("main_objects");
    return MY_OBJECTS_LIST_ID;
  }

  /// Attribute name for tolerance.
  inline static const std::string& TOLERANCE_ID()
  {
    static const std::string MY_TOLERANCE_ID("tolerance");
    return MY_TOLERANCE_ID;
  }

  /// Attribute name for keeping non-solid shapes.
  inline static const std::string& KEEP_NON_SOLIDS_ID()
  {
    static const std::string MY_KEEP_NON_SOLIDS_ID("keep_non_solids");
    return MY_KEEP_NON_SOLIDS_ID;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Executes the faces fusion and stores the modififed shape.
  FEATURESPLUGIN_EXPORT virtual void execute();

private:
  /// Retrieve all shapes from the selection list
  void getOriginalShapes(const std::string& theAttributeName,
                         ListOfShape&       theShapes);

  /// Check, whether the result is glued or not
  bool isGlued(const ListOfShape& theInputs,
               const GeomShapePtr theResult);

  /// Create a compound result object from the unchanged input shapes
  void setResultFromInput(const ListOfShape& theInputs);
};

#endif // FeaturesPlugin_GlueFaces_H_
