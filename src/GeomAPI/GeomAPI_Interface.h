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

template <typename T>
void GeomAPI_deleter( void* p ) {
   delete reinterpret_cast<T*>(p);
}

class GeomAPI_Interface
{
 private:
  std::shared_ptr<char> myImpl;  ///< pointer to the internal impl object

 public:
  /// None - constructor
  GEOMAPI_EXPORT GeomAPI_Interface();

  /// Constructor by the impl pointer (used for internal needs)
  template<class T> explicit GeomAPI_Interface(T* theImpl)
  {
    myImpl = std::shared_ptr<char>(reinterpret_cast<char*>(theImpl), GeomAPI_deleter<T>);
  }

  /// Destructor
  GEOMAPI_EXPORT virtual ~GeomAPI_Interface();

  /// Returns the pointer to the impl
  template<class T> inline T* implPtr()
  {
    return reinterpret_cast<T*>(myImpl.get());
  }
  /// Returns the pointer to the impl
  template<class T> inline const T* implPtr() const
  {
    return reinterpret_cast<T*>(myImpl.get());
  }
  /// Returns the reference object of the impl
  template<class T> inline const T& impl() const
  {
    return *(reinterpret_cast<T*>(myImpl.get()));
  }
  /// Updates the impl (deletes the old one)
  template<class T> inline void setImpl(T* theImpl)
  {
    myImpl = std::shared_ptr<char>(reinterpret_cast<char*>(theImpl), GeomAPI_deleter<T>);
  }

  /// Returns true if the impl is empty
  GEOMAPI_EXPORT bool empty() const;
};

#endif
