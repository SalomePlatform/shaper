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

#ifndef PARAMETERSPLUGIN_PARAMETERSMGR_H_
#define PARAMETERSPLUGIN_PARAMETERSMGR_H_

#include "ParametersPlugin.h"
#include <ModelAPI_Feature.h>

/**
 * \class ParametersPlugin_ParametersMgr
 * \ingroup Plugins
 * \brief A macro feature which manages list of parameters in the current document.
 */

class ParametersPlugin_ParametersMgr : public ModelAPI_Feature
{
public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_PARAMSMGR_ID("ParametersMgr");
    return MY_PARAMSMGR_ID;
  }

  /// Use plugin manager for features creation
  ParametersPlugin_ParametersMgr();

  /// Destructor
  virtual ~ParametersPlugin_ParametersMgr();

  /// Request for initialization of data model of the feature: adding all attributes
  PARAMETERSPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the unique kind of a feature
  PARAMETERSPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = ParametersPlugin_ParametersMgr::ID();
    return MY_KIND;
  };

  /// Computes or recomputes the results
  PARAMETERSPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns true.
  PARAMETERSPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  PARAMETERSPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

  /// Returns true if result is persistent (stored in document) and on undo-redo, save-open
  /// it is not needed to recompute it.
  PARAMETERSPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Returns true if this feature must not be created: this is just an action
  /// that is not stored in the features history and data model (like "delete part").
  virtual bool isInHistory()  { return false; }
};


#endif