// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef SRC_PARAMETERSPLUGIN_EVALLISTENER_H_
#define SRC_PARAMETERSPLUGIN_EVALLISTENER_H_

#include <ParametersPlugin.h>
#include <Events_Loop.h>
#include <string>
#include <memory>

class ModelAPI_Attribute;
class ModelAPI_Document;
class ModelAPI_Feature;
class ModelAPI_ResultParameter;
class ParametersPlugin_Parameter;

/**
 * \class ParametersPlugin_EvalListener
 * \ingroup Plugins
 * \brief Class which process the events from the event loop.
 */
class ParametersPlugin_EvalListener : public Events_Listener
{
  /// map of old names, that were valid for perameres, but now parameter name is changed to invalid
  /// stored not shared pointers to features to avoid memory keeping on the feature delete
  std::map<ParametersPlugin_Parameter*, std::wstring> myOldNames;
 public:
  PARAMETERSPLUGIN_EXPORT ParametersPlugin_EvalListener();
  PARAMETERSPLUGIN_EXPORT virtual ~ParametersPlugin_EvalListener();

  /// Reimplemented from Events_Listener::processEvent().
  PARAMETERSPLUGIN_EXPORT
    virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

 protected:
  /// Processes ObjectRenamed event.
  void processObjectRenamedEvent(const std::shared_ptr<Events_Message>& theMessage);
  /// Processes ReplaceParameter event.
  void processReplaceParameterEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Renames theOldName in theExpression by theNewName.
  std::wstring renameInPythonExpression(const std::wstring& theExpression,
                                        const std::wstring& theOldName,
                                        const std::wstring& theNewName);
  /// Renames theOldName in the expression attribute of theParameter by theNewName.
  void renameInParameter(std::shared_ptr<ParametersPlugin_Parameter> theParameter,
                         const std::wstring& theOldName,
                         const std::wstring& theNewName);
  /// Renames theOldName in the text fields of theAttribute by theNewName.
  void renameInAttribute(std::shared_ptr<ModelAPI_Attribute> theAttribute,
                         const std::wstring& theOldName,
                         const std::wstring& theNewName);
  /// Renames theOldName in all dependents of theResultParameter by theNewName.
  void renameInDependents(std::shared_ptr<ModelAPI_ResultParameter> theResultParameter,
                          const std::wstring& theOldName,
                          const std::wstring& theNewName);
};

#endif /* SRC_PARAMETERSPLUGIN_PARAMETERSPLUGIN_EVALLISTENER_H_ */
