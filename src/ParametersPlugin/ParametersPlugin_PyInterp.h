/*
 * ParametersPlugin_PyInterp.h
 *
 *  Created on: Apr 2, 2015
 *      Author: sbh
 */

#ifndef PARAMETERSPLUGIN_PYINTERP_H_
#define PARAMETERSPLUGIN_PYINTERP_H_

#include <PyInterp_Interp.h>
#include <ParametersPlugin.h>

#include <list>
#include <string>

class PARAMETERSPLUGIN_EXPORT ParametersPlugin_PyInterp : public PyInterp_Interp
{
 public:
  ParametersPlugin_PyInterp();
  virtual ~ParametersPlugin_PyInterp();

  std::list<std::string> compile(const std::string&);
  void extendLocalContext(const std::list<std::string>&);
  void clearLocalContext();
  double evaluate(const std::string&, std::string&);

 protected:
  std::string errorMessage();
  // Overrides PyInterp_Interp
  virtual bool initContext();
};

#endif /* PARAMETERSPLUGIN_PYINTERP_H_ */
