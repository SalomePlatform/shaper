
#ifndef PYCONSOLE_INTERP_H
#define PYCONSOLE_INTERP_H

#include "PyConsole.h"

#include <PyInterp_Interp.h>   /// !!! WARNING !!! THIS INCLUDE MUST BE VERY FIRST !!!

//PYCONSOLE_EXPORT
class PyConsole_Interp : public PyInterp_Interp
{
public:
  PyConsole_Interp();
  ~PyConsole_Interp();

protected:
  virtual bool initState();
  virtual bool initContext();  
};

#endif // PYCONSOLE_INTERP_H
