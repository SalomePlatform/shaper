// File:        GeomAPI_Interface.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Interface_HeaderFile
#define GeomAPI_Interface_HeaderFile

#include <GeomAPI.h>

/**\class GeomAPI_Interface
 * \ingroup DataModel
 * \brief General base class for all interfaces in this package
 */

class GEOMAPI_EXPORT GeomAPI_Interface
{
protected:
  void* myImpl; ///< pointer to the internal implementation object

public:
  /// None - constructor
  GeomAPI_Interface();

  /// Constructor by the implementation pointer (used for internal needs)
  GeomAPI_Interface(void* theImpl);
  
  /// Destructor
  virtual ~GeomAPI_Interface();

  /// Returns the pointer to the implementation
  void* implementation();
  /// Updates the implementation (deletes the old one)
  void setImplementation(void* theImpl);
};

#endif

