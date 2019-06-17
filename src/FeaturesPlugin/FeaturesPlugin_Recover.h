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

#ifndef FEATURESPLUGIN_RECOVER_H_
#define FEATURESPLUGIN_RECOVER_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>

/**\class FeaturesPlugin_Recover
 * \ingroup Plugins
 * \brief Feature for display of concealed attributes of some feature.
 */
class FeaturesPlugin_Recover : public ModelAPI_Feature
{
  /// List of already registered unconcealed pairs of unconcealed result and feature that caused
  /// concealment. If attributes of this feature were changed, this list helps to synchronise
  /// the current state with validators.
  std::set<std::shared_ptr<ModelAPI_Object> > myRegistered;
  /// The last stored Base to unregister Unconcealed when even attributes are already erased.
  FeaturePtr myCurrentBase;
  /// previous state of persistent flag
  bool myPersistent;
  /// necessity to clean recovered list while changing the type of recover
  bool myClearListOnTypeChange;
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_RECOVER_ID("Recover");
    return MY_RECOVER_ID;
  }
  /// Attribute name of base feature.
  inline static const std::string& BASE_FEATURE()
  {
    static const std::string MY_BASE_FEATURE("base_feature");
    return MY_BASE_FEATURE;
  }

  /// Attribute name of recovered results list.
  inline static const std::string& RECOVERED_ENTITIES()
  {
    static const std::string MY_RECOVERED_ENTITIES_ID("recovered");
    return MY_RECOVERED_ENTITIES_ID;
  }

  /// Attribute name of the type of recover.
  inline static const std::string& METHOD()
  {
    static const std::string MY_METHOD_ID("method");
    return MY_METHOD_ID;
  }

  /// Value of default method, recovering concealed objects only.
  inline static const std::string& METHOD_DEFAULT()
  {
    static const std::string MY_METHOD_ID("default");
    return MY_METHOD_ID;
  }
  /// Value of method, recovering parent compounds of concealed objects.
  inline static const std::string& METHOD_COMPOUND()
  {
    static const std::string MY_METHOD_ID("compound");
    return MY_METHOD_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Recover::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  FeaturesPlugin_Recover();

};

#endif
