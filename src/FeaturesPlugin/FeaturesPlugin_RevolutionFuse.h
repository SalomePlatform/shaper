// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionFuse.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_RevolutionFuse_H_
#define FeaturesPlugin_RevolutionFuse_H_

#include <FeaturesPlugin_RevolutionBoolean.h>

/** \class FeaturesPlugin_RevolutionFuse
 *  \ingroup Plugins
 */
class FeaturesPlugin_RevolutionFuse : public FeaturesPlugin_RevolutionBoolean
{
 public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_REVOLUTION_ID("RevolutionFuse");
    return MY_REVOLUTION_ID;
  }

  /// \return the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_RevolutionFuse::ID();
    return MY_KIND;
  }

  /// Use plugin manager for features creation.
  FeaturesPlugin_RevolutionFuse()
  {
    myBooleanOperationType = GeomAlgoAPI_Boolean::BOOL_FUSE;
  }
};

#endif
