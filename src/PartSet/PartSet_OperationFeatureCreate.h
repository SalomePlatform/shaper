// File:        PartSet_OperationFeatureCreate.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationFeatureCreate_H
#define PartSet_OperationFeatureCreate_H

#include "PartSet.h"

#include <PartSet_OperationFeatureBase.h>
#include <PartSet_Constants.h>

#include <QObject>

class GeomDataAPI_Point2D;
class QMouseEvent;
class QKeyEvent;

/*!
 \class PartSet_OperationFeatureCreate
 * \brief The operation for the sketch feature creation
 */
class PARTSET_EXPORT PartSet_OperationFeatureCreate : public PartSet_OperationFeatureBase
{
Q_OBJECT

 public:
  /// Returns true if the feature with the given kind can be created by this operation
  /// \param theId the feature kind
  /// \return the boolean result
  static bool canProcessKind(const std::string& theId);

  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  /// \param theSketch the parent feature
  PartSet_OperationFeatureCreate(const QString& theId, QObject* theParent, FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_OperationFeatureCreate();

  /// Gives the current mouse point in the viewer
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView);

  /// Gives the current selected objects to be processed by the operation
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                             const std::list<ModuleBase_ViewerPrs>& theSelected,
                             const std::list<ModuleBase_ViewerPrs>& theHighlighted);
  /// Processes the key pressed in the view
  /// \param theKey a key value
  virtual void keyReleased(const int theKey);

  /// Initializes the operation with previously created feature. It is used in sequental operations
  void initFeature(FeaturePtr theFeature) { myInitFeature = theFeature; }

 public slots:
  /// Slots which listen the mode widget activation
  /// \param theWidget the model widget
  virtual void onWidgetActivated(ModuleBase_ModelWidget* theWidget);

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
  virtual FeaturePtr createFeature(const bool theFlushMessage = true);

  /// Verifies whether this operator can be commited.
  /// \return Returns TRUE if current operation can be committed, e.g. all parameters are filled
  virtual bool canBeCommitted() const;

protected:
  /// Feature of previous operation (for sequintal operations)
  FeaturePtr myInitFeature;
};

#endif
