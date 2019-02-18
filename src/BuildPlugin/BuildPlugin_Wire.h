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

#ifndef BuildPlugin_Wire_H_
#define BuildPlugin_Wire_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_Wire
/// \ingroup Plugins
/// \brief Feature for creation of wire from sketch edges or existing wires.
class BuildPlugin_Wire: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_Wire();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Wire");
    return MY_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& ADD_CONTOUR_ACTION_ID()
  {
    static const std::string MY_ADD_CONTOUR_ACTION_ID("add_contour");
    return MY_ADD_CONTOUR_ACTION_ID;
  }

  /// \return the kind of a feature.
  BUILDPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = BuildPlugin_Wire::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();

  /// Performs some functionality by action id.
  /// \param[in] theAttributeId action key id.
  /// \return false in case if action not perfomed.
  BUILDPLUGIN_EXPORT virtual bool customAction(const std::string& theActionId);

private:
  /// Action: Adds to the list of segments other segments of the sketcher connected to
  /// the already selected ones to create a closed contour.
  /// \return false in case if no countours have been added.
  bool addContour();
};

#endif
