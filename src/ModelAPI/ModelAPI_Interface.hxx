// File:        ModelAPI_Interface.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Interface_HeaderFile
#define ModelAPI_Interface_HeaderFile

#include <ModelAPI.hxx>

/**\class ModelAPI_Interface
 * \ingroup DataModel
 * \brief General base class for all interfaces in this package
 */

class MODELAPI_EXPORT ModelAPI_Interface
{
  void* myImpl; ///< pointer to the internal implementation object

public:
  /// None - constructor
  virtual ModelAPI_Interface()
  {
    myImpl = 0;
  }

  /// Constructor by the implementation pointer (used for internal needs)
  virtual ModelAPI_Interface(void* theImpl)
  {
    myImpl = theImpl;
  }

  /// Copy-constructor
  virtual ModelAPI_Interface(ModelAPI_Interface& theOrig)
  {
    myImpl = theOrig.theImpl;
    Duplicate();
  }

  virtual ModelAPI_Interface& operator=(ModelAPI_Interface& const theOrig)
  { myImpl = theOrig.theImpl; Duplicate(); return *this;}

  /// Duplicates the objet pointed by myImpl (loosing the old one)
  virtual void Duplicate() = 0;

};

#endif
