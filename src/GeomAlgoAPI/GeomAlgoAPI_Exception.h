#ifndef GeomAlgoAPI_Exception_H_
#define GeomAlgoAPI_Exception_H_

#include <GeomAlgoAPI.h>

#include <iostream>

/**\class GeomAlgoAPI_Exception
 * \ingroup DataAlgo
 * \brief Manage exceptions
 */
class GeomAlgoAPI_Exception : public std::exception
{
 public: 
  /// Create an exception
  /// \param theMessageError Error message to be displayed
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Exception(std::string theMessageError);
  /// Destroyer 
  GEOMALGOAPI_EXPORT ~GeomAlgoAPI_Exception() throw();
  /// Allows to collet the error 
  GEOMALGOAPI_EXPORT const char* what() const throw();
 
private:
  std::string myMessageError; /// Error message to be displayed.
};


#endif