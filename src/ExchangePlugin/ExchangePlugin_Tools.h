// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_Tools.h
// Created: May 15, 2015
// Author:  Sergey POKHODENKO

#ifndef EXCHANGEPLUGIN_TOOLS_H_
#define EXCHANGEPLUGIN_TOOLS_H_

#include <ExchangePlugin.h>
#include <ModelAPI_AttributeTables.h>

#include <list>
#include <string>

/**
 * \class ExchangePlugin_Tools
 * \brief Internal tools for the ExchangePlugin.
 */
class EXCHANGEPLUGIN_EXPORT ExchangePlugin_Tools {
public:
  /// Splits theString using theDelimiter.
  static std::list<std::string> split(const std::string& theString,
                                      char theDelimiter);

  /// Converts string representation of selection type to XAO dimension.
  static std::string selectionType2xaoDimension(const std::string& theString);

  /// Converts string representation of XAO dimension to selection type.
  static std::string xaoDimension2selectionType(const std::string& theDimension);

  /// Converts representation of values type to XAO type.
  static std::string valuesType2xaoType(const ModelAPI_AttributeTables::ValueType& theType);
  /// Converts representation of values type to XAO type.
  static ModelAPI_AttributeTables::ValueType xaoType2valuesType(std::string theType);
};

#endif /* EXCHANGEPLUGIN_TOOLS_H_ */
