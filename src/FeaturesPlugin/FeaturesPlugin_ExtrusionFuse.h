// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionFuse.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_ExtrusionFuse_H_
#define FeaturesPlugin_ExtrusionFuse_H_

#include <FeaturesPlugin_ExtrusionBoolean.h>

/** \class FeaturesPlugin_ExtrusionFuse
 *  \ingroup Plugins
 *  \brief This feature allows to create sketch, extrude faces from this sketch and
 *         fuse result with other objects in a single operation.
 */
class FeaturesPlugin_ExtrusionFuse : public FeaturesPlugin_ExtrusionBoolean
{
 public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_EXTRUSION_ID("ExtrusionFuse");
    return MY_EXTRUSION_ID;
  }

  /// \return the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_ExtrusionFuse::ID();
    return MY_KIND;
  }

  /// Use plugin manager for features creation.
  FeaturesPlugin_ExtrusionFuse();
};

#endif
