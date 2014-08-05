// File:        ModuleBase_WidgetFeatureOrAttribute.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetFeatureOrAttribute_H
#define ModuleBase_WidgetFeatureOrAttribute_H

#include <ModuleBase.h>
#include "ModuleBase_WidgetFeature.h"

#include <QObject>

class ModuleBase_WidgetValue;
class ModelAPI_Attribute;

/**\class ModuleBase_WidgetFeatureOrAttribute
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class MODULEBASE_EXPORT ModuleBase_WidgetFeatureOrAttribute : public ModuleBase_WidgetFeature
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetFeatureOrAttribute(QWidget* theParent, const Config_WidgetAPI* theData, const std::string& theParentId);
  /// Destructor
  virtual ~ModuleBase_WidgetFeatureOrAttribute();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  virtual bool setValue(ModuleBase_WidgetValue* theValue);

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(ObjectPtr theFeature) const;

  virtual bool restoreValue(ObjectPtr theFeature);

protected:
  /// Set the attribute
  /// \param theAttribute value
  /// \return the boolean result of the attribute set
  bool setAttribute(const boost::shared_ptr<ModelAPI_Attribute>& theAttribute, 
                    bool theSendEvent = true);

protected:
  boost::shared_ptr<ModelAPI_Attribute> myAttribute; /// < the attribute
};

#endif
