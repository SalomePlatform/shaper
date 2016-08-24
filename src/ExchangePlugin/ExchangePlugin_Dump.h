// Copyright (C) 2016-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_Dump.h
// Created: August 1, 2016
// Author:  Artem ZHIDKOV

#ifndef EXCHANGEPLUGIN_DUMP_H_
#define EXCHANGEPLUGIN_DUMP_H_

#include <ExchangePlugin.h>

#include <ModelAPI_Feature.h>

/**
 * \class ExchangePlugin_Dump
 * \ingroup Plugins
 * \brief Store full model as a Python script
 */
class ExchangePlugin_Dump : public ModelAPI_Feature
{
public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_DUMP_ID("Dump");
    return MY_DUMP_ID;
  }
  /// attribute name of file path
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("file_path");
    return MY_FILE_PATH_ID;
  }
  /// attribute name of file format
  inline static const std::string& FILE_FORMAT_ID()
  {
    static const std::string MY_FILE_FORMAT_ID("file_format");
    return MY_FILE_FORMAT_ID;
  }

  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_Dump();
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_Dump();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind()
  {
    return ExchangePlugin_Dump::ID();
  }

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns true.
  EXCHANGEPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

protected:
  /// Performs dump to the file
  EXCHANGEPLUGIN_EXPORT void dump(const std::string& theFileName);
};

#endif
