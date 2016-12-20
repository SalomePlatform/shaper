// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->
/*
 * ParametersPlugin_EvalListener.h
 *
 *  Created on: Apr 28, 2015
 *      Author: sbh
 */

#ifndef SRC_PARAMETERSPLUGIN_EVALLISTENER_H_
#define SRC_PARAMETERSPLUGIN_EVALLISTENER_H_

#include <ParametersPlugin.h>
#include <Events_Loop.h>

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
  std::map<ParametersPlugin_Parameter*, std::string> myOldNames;
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
  std::string renameInPythonExpression(const std::string& theExpression,
                                       const std::string& theOldName,
                                       const std::string& theNewName);
  /// Renames theOldName in the expression attribute of theParameter by theNewName.
  void renameInParameter(std::shared_ptr<ParametersPlugin_Parameter> theParameter,
                         const std::string& theOldName,
                         const std::string& theNewName);
  /// Renames theOldName in the text fields of theAttribute by theNewName.
  void renameInAttribute(std::shared_ptr<ModelAPI_Attribute> theAttribute,
                         const std::string& theOldName,
                         const std::string& theNewName);
  /// Renames theOldName in all dependents of theResultParameter by theNewName.
  void renameInDependents(std::shared_ptr<ModelAPI_ResultParameter> theResultParameter,
                          const std::string& theOldName,
                          const std::string& theNewName);
};

#endif /* SRC_PARAMETERSPLUGIN_PARAMETERSPLUGIN_EVALLISTENER_H_ */
