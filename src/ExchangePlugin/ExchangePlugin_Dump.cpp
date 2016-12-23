// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_ExportFeature.cpp
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#include <ExchangePlugin_Dump.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Session.h>

#include <ModelHighAPI_Dumper.h>

#include <Config_ModuleReader.h>


ExchangePlugin_Dump::ExchangePlugin_Dump()
{
}

ExchangePlugin_Dump::~ExchangePlugin_Dump()
{
}

void ExchangePlugin_Dump::initAttributes()
{
  data()->addAttribute(ExchangePlugin_Dump::FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_Dump::FILE_FORMAT_ID(), ModelAPI_AttributeString::typeId());
}

void ExchangePlugin_Dump::execute()
{
  AttributeStringPtr aFilePathAttr =
      this->string(ExchangePlugin_Dump::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty())
    return;

  dump(aFilePath);
}

void ExchangePlugin_Dump::dump(const std::string& theFileName)
{
  // load DumpAssistant from Python side
  Config_ModuleReader::loadScript("salome.shaper.model.dump");

  ModelHighAPI_Dumper* aDumper = ModelHighAPI_Dumper::getInstance();
  aDumper->clear();
  DocumentPtr aDoc = ModelAPI_Session::get()->moduleDocument();
  if (!aDumper || !aDumper->process(aDoc, theFileName))
    setError("An error occured while dumping to " + theFileName);
}
