// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ModuleBase_WidgetOptionalBox.h
// Created:     13 Dec 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetOptionalBox_H_
#define ModuleBase_WidgetOptionalBox_H_

#include <ModuleBase.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_ModelWidget.h>

class QGroupBox;
class QFrame;
class QCheckBox;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;

/**
* \ingroup GUI
* Implements a model widget for switch as a container widget. It can be defined in XML with "toolbox" keyword
*/
class MODULEBASE_EXPORT ModuleBase_WidgetOptionalBox : public ModuleBase_ModelWidget,
                                                         public ModuleBase_PageBase
{
  Q_OBJECT

  enum OptionType {CheckBox, GroupBox};
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  ModuleBase_WidgetOptionalBox(QWidget* theParent, const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetOptionalBox();

  /// Defines if it is supported to set the value in this widget
  /// \return false because this is an info widget
  virtual bool canAcceptFocus() const { return false; };

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
  /// Sets whether Check box or Group box is used currently
  /// \param theType a type of the control
  void setOptionType(const OptionType& theType);

  /// Returns true if the model widget is set into the check box frame
  /// \return boolean value
  bool isCheckBoxFilled() const;

  /// Creates controls for the given type if it was not perfomed yet
  /// \param theType a type to create whether check box or group box controls
  void createControl(const OptionType& theType);

  /// Returns true if control is checked
  /// \return boolean value
  bool getCurrentValue() const;

  /// Returns true if control is checked
  /// \param theValue a new value to fill the control state
  void setCurrentValue(const bool& theValue);

  /// Makes model widget controls either disabled(for check box mode) or hidden(for group box mode)
  void updateControlsVisibility();

private:
  std::string myToolTip; ///< tool tip defined in XML
  std::string myGroupTitle; ///< a title for group box

  OptionType myOptionType; ///< current option state
  QVBoxLayout* myMainLayout; ///< the main layout
  // controls to fill check box frame
  QFrame* myCheckBoxFrame; ///< frame with check box, to be hidden when group box is used
  QCheckBox* myCheckBox; ///< control used if only one model widget is placed inside
  QHBoxLayout* myCheckBoxLayout; ///< layout of check box frame
  ModuleBase_ModelWidget* myCheckBoxWidget; /// model widget, placed in the check box frame
  // controls to fill group box frame
  QGroupBox* myGroupBox; ///< control used if more than model widget is placed inside
  QGridLayout* myGroupBoxLayout; ///< page's layout
};

#endif /* ModuleBase_WidgetOptionalBox_H_ */
