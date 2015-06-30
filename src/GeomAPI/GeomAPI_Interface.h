// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Interface.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Interface_H_
#define GeomAPI_Interface_H_

#include <GeomAPI.h>

#include <memory>

/**\class GeomAPI_Interface
 * \ingroup DataModel
 * \brief General base class for all interfaces in this package
 */

class GeomAPI_Interface
{
 private:
  std::shared_ptr<void> myImpl;  ///< pointer to the internal impl object

 public:
  /// None - constructor
  GEOMAPI_EXPORT GeomAPI_Interface();

  /// Constructor by the impl pointer (used for internal needs)
  template<class T> explicit GeomAPI_Interface(T* theImpl)
  {
    myImpl.reset(theImpl);
  }

  /// Destructor
  GEOMAPI_EXPORT virtual ~GeomAPI_Interface();

  /// Returns the pointer to the impl
  template<class T> inline T* implPtr()
  {
    return static_cast<T*>(myImpl.get());
  }
  /// Returns the pointer to the impl
  template<class T> inline const T* implPtr() const
  {
    return static_cast<T*>(myImpl.get());
  }
  /// Returns the reference object of the impl
  template<class T> inline const T& impl() const
  {
    return *(static_cast<T*>(myImpl.get()));
  }
  /// Updates the impl (deletes the old one)
  template<class T> inline void setImpl(T* theImpl)
  {
    myImpl.reset(theImpl);
  }

  /// Returns true if the impl is empty
  GEOMAPI_EXPORT bool empty() const;
};

#endif
