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

#ifndef SAMPLEPANELPLUGIN_FEATURE_H_
#define SAMPLEPANELPLUGIN_FEATURE_H_

#include "SamplePanelPlugin.h"

#include <ModelAPI_Feature.h>

/**\class SamplePanelPlugin_Feature
 * \ingroup Plugins
 * \brief Sample feature to be filled by the panel.
 */
class SamplePanelPlugin_Feature : public ModelAPI_Feature
{
 public:
  /// SketchShape feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_SAMPLE_PANEL_FEATURE_ID("QtPanelFeature");
    return MY_SAMPLE_PANEL_FEATURE_ID;
  }

  /// Total number of objects, initial and translated objects
  inline static const std::string& VALUE_ID()
  {
    static const std::string MY_VALUE_ID("Value");
    return MY_VALUE_ID;
  }

  /// Request for initialization of data model of the object: adding all attributes
  virtual void initAttributes();

  /// Returns the unique kind of a feature
  virtual const std::string& getKind() {
    static std::string MY_KIND = SamplePanelPlugin_Feature::ID();
    return MY_KIND;
  };

  /// Computes or recomputes the results
  virtual void execute() {}

  /// Use plugin manager for features creation
  SamplePanelPlugin_Feature();
};

typedef std::shared_ptr<SamplePanelPlugin_Feature> SamplePanelFeaturePtr;

#endif
