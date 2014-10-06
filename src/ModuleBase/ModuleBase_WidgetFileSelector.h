/*
 * ModuleBase_WidgetFileSelector.h
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETFILESELECTOR_H_
#define MODULEBASE_WIDGETFILESELECTOR_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <QList>
#include <QString>
#include <QStringList>

class QWidget;
class QLineEdit;

class MODULEBASE_EXPORT ModuleBase_WidgetFileSelector : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  ModuleBase_WidgetFileSelector(QWidget* theParent,
                                const Config_WidgetAPI* theData,
                                const std::string& theParentId);
  virtual ~ModuleBase_WidgetFileSelector();

  /// Saves the internal parameters to the given feature
  /// \param theObject a model feature to be changed
  virtual bool storeValue() const;

  virtual bool restoreValue();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns true if a file on the current path in the line edit
  /// exists and has supported format
  bool isCurrentPathValid();

 public slots:
  void onPathSelectionBtn();
  void onPathChanged();

 protected:
  QString formatsString() const;
  QStringList getValidatorFormats() const;

 private:
  QLineEdit* myPathField;
  QWidget* myMainWidget;

  QString myTitle;
  QString myDefaultPath;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
