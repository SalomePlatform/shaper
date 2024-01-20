// Copyright (C) 2017-2024  CEA, EDF
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

#ifndef FeaturesPlugin_Fillet_H_
#define FeaturesPlugin_Fillet_H_

#include "FeaturesPlugin_VersionedChFi.h"

/// \class FeaturesPlugin_Fillet
/// \ingroup Plugins
/// \brief Feature for applying the Fillet operations on Solids.
///        Supports fixed radius fillet and varying 2-radius fillet.
class FeaturesPlugin_Fillet : public FeaturesPlugin_VersionedChFi
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Fillet");
    return MY_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Fillet::ID();
    return MY_KIND;
  }

  inline static const std::string& CREATION_METHOD()
  {
    static std::string MY_CREATION_METHOD("creation_method");
    return MY_CREATION_METHOD;
  }

  inline static const std::string CREATION_METHOD_SINGLE_RADIUS()
  {
    static std::string MY_SINGLE_RADIUS("fixed_radius");
    return MY_SINGLE_RADIUS;
  }

  inline static const std::string CREATION_METHOD_VARYING_RADIUS()
  {
    static std::string MY_VARYING_RADIUS("variable_radius");
    return MY_VARYING_RADIUS;
  }

  /// Attribute name of main objects.
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }

  /// Attribute name of start radius.
  inline static const std::string& START_RADIUS_ID()
  {
    static const std::string MY_START_RADIUS_ID("radius1");
    return MY_START_RADIUS_ID;
  }

  /// Attribute name of end radius.
  inline static const std::string& END_RADIUS_ID()
  {
    static const std::string MY_END_RADIUS_ID("radius2");
    return MY_END_RADIUS_ID;
  }

  /// Attribute name of a radius for fillet with fixed radius.
  inline static const std::string& RADIUS_ID()
  {
    return START_RADIUS_ID();
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Fillet();

private:
  /// Return attribute storing the selected objects of the operation.
  virtual std::shared_ptr<ModelAPI_Attribute> objectsAttribute();

  /// Return name of modified shape prefix name
  virtual const std::string& modifiedShapePrefix() const;

  /// Run chamfer/fillet operation and returns the modification algorithm if succeed.
  virtual std::shared_ptr<GeomAlgoAPI_MakeShape>
      performOperation(const GeomShapePtr& theSolid, const ListOfShape& theEdges);
};

#endif
