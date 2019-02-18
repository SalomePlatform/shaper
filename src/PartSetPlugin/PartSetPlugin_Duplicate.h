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

#ifndef PartSetPlugin_Duplicate_H_
#define PartSetPlugin_Duplicate_H_

#include "PartSetPlugin.h"
#include <ModelAPI_Feature.h>

/**\class PartSetPlugin_Duplicate
 * \ingroup Plugins
 * \brief Duplicates the active part (not root). Creates a new "part" feature.
 */
class PartSetPlugin_Duplicate : public ModelAPI_Feature
{
 public:

  /// Duplicate kind
  inline static const std::string& ID()
  {
    static const std::string MY_REMOVE_KIND("Duplicate");
    return MY_REMOVE_KIND;
  }
//LCOV_EXCL_START
  /// Returns the kind of a feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PartSetPlugin_Duplicate::ID();
    return MY_KIND;
  }
//LCOV_EXCL_STOP

  /// Returns to which group in the document must be added feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getGroup()
  {
    static std::string MY_GROUP = "Parts";
    return MY_GROUP;
  }

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes() {}

  /// Not normal feature that stored in the tree
  PARTSETPLUGIN_EXPORT virtual bool isAction()
  {
    return true;
  }

  /// Performs the "duplicate"
  PARTSETPLUGIN_EXPORT virtual void execute();

  /// Use plugin manager for features creation
  PartSetPlugin_Duplicate()
  {
  }

};

#endif
