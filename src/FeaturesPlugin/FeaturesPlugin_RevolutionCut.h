// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionCut.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_RevolutionCut_H_
#define FeaturesPlugin_RevolutionCut_H_

#include "FeaturesPlugin_RevolutionBoolean.h"

/// \class FeaturesPlugin_RevolutionCut
/// \ingroup Plugins
/// \brief This feature allows to create sketch, revol faces from this sketch and
///        cut result from other objects in a single operation.
class FeaturesPlugin_RevolutionCut : public FeaturesPlugin_RevolutionBoolean
{
public:
  /// Use plugin manager for features creation.
  FeaturesPlugin_RevolutionCut();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_REVOLUTION_ID("RevolutionCut");
    return MY_REVOLUTION_ID;
  }

  /// \return the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_RevolutionCut::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();
};

#endif
