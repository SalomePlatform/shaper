

#ifndef PYCONSOLE_ENHINTERP_H_
#define PYCONSOLE_ENHINTERP_H_

#include "PyConsole.h"

#include <Python.h>
#include "PyConsole_Interp.h"

#include <vector>
#include <QString>

/**
 * Enhanced Python interpreter used for auto-completion.
 * This extends PyConsole_Interp with an API wrapping the Python dir() command nicely.
 */
class PYCONSOLE_EXPORT PyConsole_EnhInterp: public PyConsole_Interp
{
public:
  PyConsole_EnhInterp()
    : PyConsole_Interp(), _last_matches(0), _doc_str("")
    {}

  virtual ~PyConsole_EnhInterp() {}

  const std::vector<QString>& getLastMatches() const { return _last_matches; }
  const QString & getDocStr() const                  { return _doc_str; }

  virtual int runDirCommand(const QString& dirArgument, const QString& startMatch);
  virtual void clearCompletion();

protected:
  /** Hard coded list of Python keywords */
  static const std::vector<QString> PYTHON_KEYWORDS;

  /** Last computed matches */
  std::vector<QString> _last_matches;
  /** Doc string of the first match - when only one match it will be displayed by the Editor*/
  QString _doc_str;

  virtual int runDirAndExtract(const QString& dirArgument, const QString & startMatch,
      std::vector<QString> & result, bool discardSwig=true) const;

};

#endif /* PYCONSOLE_ENHINTERP_H_ */
