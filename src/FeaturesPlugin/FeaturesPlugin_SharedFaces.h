// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_SharedFaces_H_
#define FeaturesPlugin_SharedFaces_H_

#include <FeaturesPlugin_CommonSharedFaces.h>

/// \class FeaturesPlugin_SharedFaces
/// \ingroup Plugins
/// \brief Feature to check the shared faces of compsolid or compound

class FeaturesPlugin_SharedFaces : public FeaturesPlugin_CommonSharedFaces
{
public:
  /// Shared faces kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Shared_faces_macro");
    return MY_ID;
  }

  /// Attribute name for object selected.
  inline static const std::string& OBJECT_ID()
  {
    static const std::string MY_OBJECT_ID("main_object");
    return MY_OBJECT_ID;
  }

  /// Attribute name for number of faces.
  inline static const std::string& NUMBER_FACES_ID()
  {
    static const std::string MY_NUMBER_FACES_ID("number_shared_faces");
    return MY_NUMBER_FACES_ID;
  }

  /// Attribute name for z coodinate.
  inline static const std::string& LIST_FACES_ID()
  {
    static const std::string MY_LIST_FACES_ID("group_list");
    return MY_LIST_FACES_ID;
  }

  /// Attribute name for transparency.
  inline static const std::string& TRANSPARENCY_ID()
  {
    static const std::string MY_TRANSPARENCY_ID("transparency");
    return MY_TRANSPARENCY_ID;
  }

  /// Attribute name for checkbox create group.
  inline static const std::string& CREATE_GROUP_ID()
  {
    static const std::string MY_CREATE_GROUP_ID("create_group");
    return MY_CREATE_GROUP_ID;
  }

  /// Attribute name for indicate to launch the algo.
  inline static const std::string& COMPUTE_ID()
  {
    static const std::string MY_COMPUTE_ID("compute");
    return MY_COMPUTE_ID;
  }

  /// Attribute name for group name.
  inline static const std::string& GROUP_NAME_ID()
  {
    static const std::string MY_GROUP_NAME_ID("group_name");
    return MY_GROUP_NAME_ID;
  }

  /// \return the kind of a feature.
  virtual const std::string& getKind()
  {
    return ID();
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns true.
  FEATURESPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Use plugin manager for features creation
  FeaturesPlugin_SharedFaces();

private:
  /// Return attribut values of object.
  virtual AttributePtr attributObject();

  /// Return attribut values of list of faces.
  virtual AttributePtr attributListFaces();

  /// Return attribut values of number of faces.
  virtual AttributePtr attributNumberFaces();

  /// Return attribut values of IsCompute.
  virtual AttributePtr attributIsCompute();

  /// Create group
  void createGroup();

  /// Update group
  void updateGroup();

  /// Feature to create group
  FeaturePtr myCreateGroupFeature;
};

#endif
