// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ModuleBase_WidgetCheckGroupBox.h
// Created:     13 Dec 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetCheckGroupBox_H_
#define ModuleBase_WidgetCheckGroupBox_H_

#include <ModuleBase.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_ModelWidget.h>

class QGroupBox;
class QGridLayout;

/**
* \ingroup GUI
* Implements a model widget for switch as a container widget. It can be defined in XML with "toolbox" keyword
*/
class MODULEBASE_EXPORT ModuleBase_WidgetCheckGroupBox : public ModuleBase_ModelWidget,
                                                         public ModuleBase_PageBase
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  ModuleBase_WidgetCheckGroupBox(QWidget* theParent, const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetCheckGroupBox();

  void setTitle(const QString& theTitle);

  /// Methods to be redefined from ModuleBase_PageBase: start
  /// Cast the page to regular QWidget
  virtual QWidget* pageWidget();
  /// Methods to be redefined from ModuleBase_PageBase: end

  /// Methods to be redefined from ModuleBase_ModelWidget: start
  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;
  /// Methods to be redefined from ModuleBase_ModelWidget: end

protected slots:
  // store value to the model
  void onPageClicked();

protected:
  /// Methods to be redefined from ModuleBase_PageBase: start
  /// Adds the given widget to page's layout
  virtual void placeModelWidget(ModuleBase_ModelWidget* theWidget);
  /// Adds the given page to page's layout
  virtual void placeWidget(QWidget* theWidget);
  /// Returns page's layout (QGridLayout)
  virtual QLayout* pageLayout();
  /// Adds a stretch to page's layout
  virtual void addPageStretch();
  /// Methods to be redefined from ModuleBase_PageBase: end


  /// Methods to be redefined from ModuleBase_ModelWidget: start
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();
  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();
  /// Methods to be redefined from ModuleBase_ModelWidget: end

private:
  QGroupBox* myGroupBox;
  QGridLayout* myMainLayout; ///< page's layout
};

#endif /* ModuleBase_WidgetCheckGroupBox_H_ */
