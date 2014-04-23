

#include "PyInterp_Event.h"
#include "PyInterp_Request.h"

void PyInterp_ExecuteEvent::Execute()
{
  myRequest->execute();
}
