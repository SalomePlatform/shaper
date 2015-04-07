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
  double evaluate(const std::string&, std::string&);

 protected:
  std::list<std::string> _compile(const std::string&);
  void _extendLocalContext(const std::list<std::string>&);
  double _evaluate(const std::string&, std::string&);
  std::string errorMessage();
};

#endif /* PARAMETERSPLUGIN_PYINTERP_H_ */
