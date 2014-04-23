#ifndef PartSet_OperationSketch_H
#define PartSet_OperationSketch_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <QObject>

/*!
 \class PartSet_OperationSketch
 * \brief The operation for the sketch creation
 *
 *  Base class for all operations. If you perform an action it is reasonable to create
*/
class PARTSET_EXPORT PartSet_OperationSketch : public PartSet_OperationSketchBase
{
Q_OBJECT
public:
  PartSet_OperationSketch(const QString& theId, QObject* theParent);
  virtual ~PartSet_OperationSketch();
};

#endif
