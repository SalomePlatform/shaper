// File:        GeomAPI_Interface.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Interface_H_
#define GeomAPI_Interface_H_

#include <GeomAPI.h>

/**\class GeomAPI_Interface
 * \ingroup DataModel
 * \brief General base class for all interfaces in this package
 */

class GEOMAPI_EXPORT GeomAPI_Interface
{
protected:
  void* myImpl; ///< pointer to the internal impl object

public:
  /// None - constructor
  GeomAPI_Interface();

  /// Constructor by the impl pointer (used for internal needs)
  GeomAPI_Interface(void* theImpl);
  
  /// Destructor
  virtual ~GeomAPI_Interface();

  /// Returns the pointer to the impl
  template<class T> inline T* implPtr() {return static_cast<T*>(myImpl);}
  /// Returns the reference object of the impl
  template<class T> inline const T& impl() {return *(static_cast<T*>(myImpl));}
  /// Updates the impl (deletes the old one)
  void setImpl(void* theImpl);
};

#endif
