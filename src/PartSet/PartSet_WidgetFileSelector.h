// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * PartSet_WidgetFileSelector.h
 *
 *  Created on: May 18, 2015
 *      Author: spo
 */

#ifndef PARTSET_WIDGETFILESELECTOR_H_
#define PARTSET_WIDGETFILESELECTOR_H_

#include "PartSet.h"

#include <ModuleBase_WidgetFileSelector.h>

class ModuleBase_IWorkshop;

/**
* \ingroup Modules
* Customization of ModuleBase_WidgetFileSelector in order to write
* format of exported file.
*/
class PARTSET_EXPORT PartSet_WidgetFileSelector : public ModuleBase_WidgetFileSelector
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetFileSelector(QWidget* theParent,
                             ModuleBase_IWorkshop* theWorkshop,
                             const Config_WidgetAPI* theData,
                             const std::string& theParentId);

  virtual ~PartSet_WidgetFileSelector() {}

protected:
  /// Reimplemented from ModuleBase_WidgetFileSelector::storeValueCustom()
  virtual bool storeValueCustom() const;

  /// Reimplemented from ModuleBase_WidgetFileSelector::restoreValue()
  virtual bool restoreValueCustom();

  /// Returns a full format string for the short format
  QString shortFormatToFullFormat( const QString & theShortFormat ) const;

private:
  ModuleBase_IWorkshop* myWorkshop; // the current workshop
};

#endif /* PARTSET_WIDGETFILESELECTOR_H_ */
