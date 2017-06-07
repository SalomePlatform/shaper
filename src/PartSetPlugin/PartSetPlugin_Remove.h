// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef PartSetPlugin_Remove_H_
#define PartSetPlugin_Remove_H_

#include "PartSetPlugin.h"
#include <ModelAPI_Feature.h>

/**\class PartSetPlugin_Remove
 * \ingroup Plugins
 * \brief Feature for creation of the new part in PartSet.
 */
class PartSetPlugin_Remove : public ModelAPI_Feature
{
 public:
  /// Remove kind
  inline static const std::string& ID()
  {
    static const std::string MY_REMOVE_KIND("Remove");
    return MY_REMOVE_KIND;
  }
  /// Returns the kind of a feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PartSetPlugin_Remove::ID();
    return MY_KIND;
  }

  /// Returns to which group in the document must be added feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getGroup()
  {
    static std::string MY_GROUP = "Parts";
    return MY_GROUP;
  }

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes()
  {
  }

  /// Not normal feature that stored in the tree
  PARTSETPLUGIN_EXPORT virtual bool isAction()
  {
    return true;
  }

  /// Performs the "remove"
  PARTSETPLUGIN_EXPORT virtual void execute();

  /// Use plugin manager for features creation
  PartSetPlugin_Remove()
  {
  }
};

#endif
