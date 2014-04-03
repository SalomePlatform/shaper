

#ifndef PYCONSOLE_EVENT_H
#define PYCONSOLE_EVENT_H

#include "PyConsole.h"

#include <QEvent>
#include <QString>

/*!
  \class PrintEvent
  \brief Python command output backend event.
  \internal
*/
class PrintEvent : public QEvent
{
public:
  static const int EVENT_ID = 65432;

  /*!
    \brief Constructor
    \param c message text (python trace)
    \param isError default to false - if true indicates that an error is being printed.
  */
  PrintEvent( const char* c, bool isError = false) :
    QEvent( (QEvent::Type)EVENT_ID ), myText( c ), errorFlag(isError)
  {}

  /*!
    \brief Get message
    \return message text (python trace)
  */
  QString text() const { return myText; }

  /**
   * @return true if this is an error message
   */
  bool isError() const { return errorFlag; }

protected:
  QString myText; //!< Event message (python trace)

  /** Set to true if an error msg is to be displayed */
  bool errorFlag;
};

#endif // PYCONSOLE_EVENT_H
