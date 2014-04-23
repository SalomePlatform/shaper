#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Feature.h>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_OperationSketch::PartSet_OperationSketch(const QString& theId,
	                                             QObject* theParent)
: PartSet_OperationSketchBase(theId, theParent)
{
}

/*!
 * \brief Destructor
 */
PartSet_OperationSketch::~PartSet_OperationSketch()
{
}

/**
 * The sketch can not be created immediately, firstly a plane should be set
 * \return false value
 */
bool PartSet_OperationSketch::isPerformedImmediately() const
{
  return false;
}
