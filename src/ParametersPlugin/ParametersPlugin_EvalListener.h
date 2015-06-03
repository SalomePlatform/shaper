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

class ParametersPlugin_PyInterp;

class PARAMETERSPLUGIN_EXPORT ParametersPlugin_EvalListener : public Events_Listener
{
 public:
  ParametersPlugin_EvalListener();
  virtual ~ParametersPlugin_EvalListener();

  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

 protected:
  double evaluate(const std::string& theExpression,
                  std::string& theError) ;

 private:
  std::shared_ptr<ParametersPlugin_PyInterp> myInterp;
};

#endif /* SRC_PARAMETERSPLUGIN_PARAMETERSPLUGIN_EVALLISTENER_H_ */
