// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef COLLECTIONPLUGIN_FIELD_H_
#define COLLECTIONPLUGIN_FIELD_H_

#include "CollectionPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>

/**\class CollectionPlugin_Field
 * \ingroup Plugins
 * \brief Feature for selection of sub-shapes of some shapes and assigning data on them.
 *
 * There is a set of containers that contains the following data that may be inserted by the user:
 * 
 * Selected sub-shapes (like in Groups).
 * Number of components for each selection.
 * Components names
 * Type of values: Boolean, Integer, Double or String.
 * Number of steps: for each step (indexes are zero-based) the values below are multiplied.
 * Stamp: integer identifier per each step
 * Defaul values for each components
 * Values for each component and each selected sub-shape: all are of specific type.
 */
class CollectionPlugin_Field : public ModelAPI_Feature
{
 public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_FIELD_ID("Field");
    return MY_FIELD_ID;
  }
  /// attribute name of selected entities list
  inline static const std::string& SELECTED_ID()
  {
    static const std::string MY_SELECTED_ID("selected");
    return MY_SELECTED_ID;
  }
  /// attribute name of componenets titles array
  inline static const std::string& COMPONENTS_NAMES_ID()
  {
    static const std::string MY_COMPONENTS_NAMES_ID("components_names");
    return MY_COMPONENTS_NAMES_ID;
  }
  /// attribute name of stamps integer array
  inline static const std::string& STAMPS_ID()
  {
    static const std::string MY_STAMPS_ID("stamps");
    return MY_STAMPS_ID;
  }
  /// attribute name of list of tables that contain deafult values (row 0) and the custom values
  inline static const std::string& VALUES_ID()
  {
    static const std::string MY_VALUES_ID("values");
    return MY_VALUES_ID;
  }

  /// Returns the kind of a feature
  COLLECTIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = CollectionPlugin_Field::ID();
    return MY_KIND;
  }

  /// Creates a new field result if needed
  COLLECTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  COLLECTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Result of fields is created on the fly and don't stored to the document
  COLLECTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  CollectionPlugin_Field();

};

#endif
