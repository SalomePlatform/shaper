// File:        GeomAPI_IPresentable.hxx
// Created:     17 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef GeomAPI_IPresentable_H_
#define GeomAPI_IPresentable_H_

#include "GeomAPI_AISObject.h"

/**
* A class which defines an interface of object which is able to create its own presentation
*/ 
class GeomAPI_IPresentable
{
public:
  /** Returns the AIS preview
  *   \param thePrevious - defines a presentation if it was created previously
  */
  virtual boost::shared_ptr<GeomAPI_AISObject> getAISObject(
                            boost::shared_ptr<GeomAPI_AISObject> thePrevious) = 0;
};

typedef boost::shared_ptr<GeomAPI_IPresentable> GeomPresentablePtr;

#endif