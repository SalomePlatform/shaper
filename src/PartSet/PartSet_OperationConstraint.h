// File:        PartSet_OperationConstraint.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationConstraint_H
#define PartSet_OperationConstraint_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <QObject>

/*!
 \class PartSet_OperationConstraint
 * \brief The operation for the sketch constraint feature creation
*/
class PARTSET_EXPORT PartSet_OperationConstraint : public PartSet_OperationSketchBase
{
  Q_OBJECT

public:
  /// Returns the operation type key
  static std::string Type() { return "SketchConstraintLength"; }

public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  /// \param theFeature the parent feature
  PartSet_OperationConstraint(const QString& theId, QObject* theParent,
                              boost::shared_ptr<ModelAPI_Feature> theSketchFeature);
  /// Destructor
  virtual ~PartSet_OperationConstraint();

  /// Returns that this operator can be started above already running one.
   /// The runned operation should be the sketch feature modified operation
  /// \param theOperation the previous running operation
  virtual bool isGranted(ModuleBase_IOperation* theOperation) const;

  /// Initializes some fields accorging to the feature
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void init(boost::shared_ptr<ModelAPI_Feature> theFeature,
                    const std::list<XGUI_ViewerPrs>& theSelected,
                    const std::list<XGUI_ViewerPrs>& theHighlighted);

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual boost::shared_ptr<ModelAPI_Feature> sketch() const;

  /// Gives the current selected objects to be processed by the operation
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
 virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                            const std::list<XGUI_ViewerPrs>& theSelected,
                            const std::list<XGUI_ViewerPrs>& theHighlighted);
  /// Gives the current mouse point in the viewer
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView);

protected:
  /// \brief Virtual method called when operation is started
  /// Virtual method called when operation started (see start() method for more description)
  /// After the parent operation body perform, set sketch feature to the created line feature
  virtual void startOperation();

  /// Virtual method called when operation aborted (see abort() method for more description)
  /// Before the feature is aborted, it should be hidden from the viewer
  virtual void abortOperation();

  /// Virtual method called when operation stopped - committed or aborted.
  /// Restore the multi selection state
  virtual void stopOperation();

  /// Virtual method called after operation committed (see commit() method for more description)
  virtual void afterCommitOperation();

  /// Creates an operation new feature
  /// In addition to the default realization it appends the created line feature to
  /// the sketch feature
  /// \param theFlushMessage the flag whether the create message should be flushed
  /// \returns the created feature
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(const bool theFlushMessage = true);

  /// Set the feature for the constraint
  /// \param theFeature the line feature
  void setFeature(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Set the value for the constraint
  /// \param theValue the constraint value
  void setValue(const double theValue);

private:
  boost::shared_ptr<ModelAPI_Feature> mySketch; ///< the sketch feature
};

#endif
