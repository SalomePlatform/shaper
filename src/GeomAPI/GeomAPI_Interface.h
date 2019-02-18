// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
