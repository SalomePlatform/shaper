// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef SketchShapePlugin_WidgetCreator_H
#define SketchShapePlugin_WidgetCreator_H

#include <ModuleBase_IWidgetCreator.h>

#include <string>
#include <set>

class ModuleBase_ModelWidget;
class QWidget;

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class SketchShapePlugin_WidgetCreator : public ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  SketchShapePlugin_WidgetCreator();

  /// Virtual destructor
  ~SketchShapePlugin_WidgetCreator() {}

  /// Returns a list of possible widget types, which this creator can process
  /// \return theTypes
  virtual const std::set<std::string>& widgetTypes();

  /// Create widget by its type
  /// \param theType a type
  /// \param theParent a parent widget
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent = NULL);

private:
  std::set<std::string> myTypes; /// types of widgets
};

typedef std::shared_ptr<SketchShapePlugin_WidgetCreator> SketchShapePlguinWidgetCreatorPtr;

#endif
