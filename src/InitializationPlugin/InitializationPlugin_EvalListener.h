// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->
/*
 * InitializationPlugin_EvalListener.h
 *
 *  Created on: Apr 28, 2015
 *      Author: sbh
 */

#ifndef SRC_INITIALIZATIONPLUGIN_EVALLISTENER_H_
#define SRC_INITIALIZATIONPLUGIN_EVALLISTENER_H_

#include <InitializationPlugin.h>
#include <Events_Loop.h>

class ModelAPI_Attribute;
class ModelAPI_Document;
class ModelAPI_Feature;
class ModelAPI_ResultParameter;
class InitializationPlugin_Parameter;
class InitializationPlugin_PyInterp;

/**
 * \class InitializationPlugin_EvalListener
 * \ingroup Plugins
 * \brief Class which process the events from the event loop.
 */
class InitializationPlugin_EvalListener : public Events_Listener
{
 public:
  INITIALIZATIONPLUGIN_EXPORT InitializationPlugin_EvalListener();
  INITIALIZATIONPLUGIN_EXPORT virtual ~InitializationPlugin_EvalListener();

  /// Reimplemented from Events_Listener::processEvent().
  INITIALIZATIONPLUGIN_EXPORT
    virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

 protected:
  /// Evaluates theExpression and returns its value.
   double evaluate(std::shared_ptr<ModelAPI_Feature> theParameter,
                  const std::string& theExpression, std::string& theError,
                  std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
                  const bool theIsParameter = false);

  /// Processes Evaluation event.
  void processEvaluationEvent(const std::shared_ptr<Events_Message>& theMessage);

 private:
  std::shared_ptr<InitializationPlugin_PyInterp> myInterp;
};

#endif /* SRC_INITIALIZATIONPLUGIN_INITIALIZATIONPLUGIN_EVALLISTENER_H_ */
