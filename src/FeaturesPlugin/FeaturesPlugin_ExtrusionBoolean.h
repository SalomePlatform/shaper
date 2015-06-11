// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionBoolean.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_ExtrusionBoolean_H_
#define FeaturesPlugin_ExtrusionBoolean_H_

#include <FeaturesPlugin_CompositeBoolean.h>

/** \class FeaturesPlugin_ExtrusionBoolean
 *  \ingroup Plugins
 */
class FeaturesPlugin_ExtrusionBoolean : public FeaturesPlugin_CompositeBoolean
{
 public:
  /// Attribute name of an object from which the extrusion grows.
  inline static const std::string& FROM_OBJECT_ID()
  {
    static const std::string MY_FROM_OBJECT_ID("from_object");
    return MY_FROM_OBJECT_ID;
  }

  /// Attribute name of extrusion from size.
  inline static const std::string& FROM_SIZE_ID()
  {
    static const std::string MY_FROM_SIZE_ID("from_size");
    return MY_FROM_SIZE_ID;
  }

  /// attribute name of an object to which the extrusion grows.
  inline static const std::string& TO_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("to_object");
    return MY_TO_OBJECT_ID;
  }

  /// Attribute name of extrusion to size.
  inline static const std::string& TO_SIZE_ID()
  {
    static const std::string MY_TO_SIZE_ID("to_size");
    return MY_TO_SIZE_ID;
  }

protected:
  /// Init attributes for extrusion.
  virtual void initMakeSolidsAttributes();

  /// Create solids from faces with extrusion.
  virtual ListOfShape MakeSolids(const ListOfShape& theFaces);

protected:
  FeaturesPlugin_ExtrusionBoolean(){};
};

#endif
