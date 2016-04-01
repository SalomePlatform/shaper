// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_PageGroupBox.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef SAMPLEPANELPLUGIN_PANEL_H_
#define SAMPLEPANELPLUGIN_PANEL_H_

#include <QWidget>

#include <ModelAPI_Feature.h>

class QComboBox;

/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of some feature.
 */
class SamplePanelPlugin_Panel : public QWidget
{
 Q_OBJECT
public:
  /// Constructs a panel page
  SamplePanelPlugin_Panel(QWidget* theParent);
  /// Destructs the page
  virtual ~SamplePanelPlugin_Panel() {}

  /// Fill the panel by the feature
  /// \param theFeature a feature to fill the panel controls
  void setFeature(const FeaturePtr& theFeature);

protected slots:
  /// Update feature attribute by the current value of combo box
  /// \param theIndex a combo box modified value
  void onCurrentIndexChanged(int theIndex);

private:
  FeaturePtr myFeature; // the feature which corresponds to the current panel
  QComboBox* myComboBox; // control for value attribute of the current feature
  int myDefaultValue; /// the default combo box value
};

#endif /* SAMPLEPANELPLUGIN_PANEL_H_ */
