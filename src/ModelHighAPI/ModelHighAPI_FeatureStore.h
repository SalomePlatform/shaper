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

#ifndef ModelHighAPI_FeatureStore_H_
#define ModelHighAPI_FeatureStore_H_

#include "ModelHighAPI.h"

#include <map>
#include <list>
#include <string>
#include <memory>

class ModelAPI_Object;
class ModelAPI_Data;
class GeomAPI_Shape;
class ModelAPI_Attribute;

typedef std::shared_ptr<ModelAPI_Object>    ObjectPtr;
typedef std::shared_ptr<ModelAPI_Attribute> AttributePtr;

/**\class ModelHighAPI_FeatureStore
 * \ingroup CPPHighAPI
 * \brief Allows to compare the original and the dumped/executed feature
 */
class ModelHighAPI_FeatureStore {
  /// dumps of attributes of the stored feature (id -> dump)
  std::map<std::string, std::string> myAttrs;
   /// dumps of attributes of results (list of results id -> dumps)
  std::list<std::map<std::string, std::string> > myRes;
public:
  // unused constructor for the map container needs
  ModelHighAPI_FeatureStore() {}
  // constructor that initializes this object by feature to store
  ModelHighAPI_FeatureStore(ObjectPtr theObject);
  // compares the stored feature information with the given feature
  std::string compare(ObjectPtr theObject);

private:
  /// stores the information about all attributes of data in map
  void storeData(std::shared_ptr<ModelAPI_Data> theData,
                 std::map<std::string, std::string>& theAttrs);
  /// compares the information about all attributes of data with map
  /// returns not empty string with error if something is different
  std::string compareData(std::shared_ptr<ModelAPI_Data> theData,
                 std::map<std::string, std::string>& theAttrs);
  /// dumps the attribute content to the string
  std::string dumpAttr(const AttributePtr& theAttr);
  /// dumps the shape main charatceristics to string
  std::string dumpShape(std::shared_ptr<GeomAPI_Shape>& theShape);
};

#endif