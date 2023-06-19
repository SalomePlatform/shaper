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

#ifndef BuildPlugin_Polyline_H_
#define BuildPlugin_Polyline_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_Polyline
/// \ingroup Plugins
/// \brief Feature for creation of polyline from set of points.
class BuildPlugin_Polyline : public ModelAPI_Feature
{
public:
	/// Use plugin manager for features creation
	BuildPlugin_Polyline();

	/// Feature kind.
	inline static const std::string& ID()
	{
		static const std::string MY_ID("Polyline");
		return MY_ID;
	}

	/// Attribute name of base objects.
	inline static const std::string& BASE_OBJECTS_ID()
	{
		static const std::string MY_BASE_OBJECTS_ID("base_objects");
		return MY_BASE_OBJECTS_ID;
	}

  /// Attribute name of closed flag.
  inline static const std::string& CLOSED_ID()
  {
    static const std::string MY_CLOSED_ID("closed");
    return MY_CLOSED_ID;
  }

  /// Default value of the closed attribute
  inline static bool CLOSED_DEFAULT() { return false; }

	/// \return the kind of a feature.
	BUILDPLUGIN_EXPORT virtual const std::string& getKind()
	{
		static std::string MY_KIND = BuildPlugin_Polyline::ID();
		return MY_KIND;
	}

	/// Request for initialization of data model of the feature: adding all attributes.
	BUILDPLUGIN_EXPORT virtual void initAttributes();

	/// Creates a new part document if needed.
	BUILDPLUGIN_EXPORT virtual void execute();
};

#endif
