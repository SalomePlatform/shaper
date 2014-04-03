


#include "PyConsole.h"

#include "PyConsole_EnhInterp.h"

#include <pythonrun.h>
#include <string>
#include <QRegExp>

static const char * tmp_k[] = {"and",  "as", "assert", "break",  "class",
    "continue", "def",  "del",
    "elif", "else", "except", "exec", "finally",  "for",  "from", "global", "if",
    "import", "in", "is", "lambda", "not",  "or", "pass", "print",  "raise",
    "return", "try",  "while",  "with", "yield"};

const std::vector<QString> PyConsole_EnhInterp::PYTHON_KEYWORDS = \
      std::vector<QString>(tmp_k, tmp_k+sizeof(tmp_k)/sizeof(tmp_k[0]));

/*!
  \brief Run Python dir() command and saves the result internally in _lastPy
  \param dirArgument Python expression to pass to the dir command. The parsing of what the
  user actually started typing is dedicated to the caller
  \param startMatch string representing the begining of the patter to be completed. For example when
  the user types "a_string_variable.rsp <TAB>", this is "rsp".
  \return command exit status - 0 = success
*/
int PyConsole_EnhInterp::runDirCommand(const QString & dirArgument, const QString & startMatch)
{
  int ret;
  std::vector<QString> v;

  clearCompletion();
  if ( (ret = runDirAndExtract(dirArgument, startMatch, _last_matches)) )
      return ret;

  // If dirArgument is empty, we append the __builtins__
  if (dirArgument.isEmpty())
    {
      if ( (ret = runDirAndExtract(QString("__builtins__"), startMatch, _last_matches, false)) )
            return ret;

      // ... and we match on Python's keywords as well:
      for (std::vector<QString>::const_iterator it = PYTHON_KEYWORDS.begin(); it != PYTHON_KEYWORDS.end(); it++)
          if ((*it).startsWith(startMatch))
            _last_matches.push_back(*it);
    }

  // Try to get doc string of the first match
  if (_last_matches.size() > 0)
    {
      QString cmd("");
      if (dirArgument.trimmed() != "")
        cmd = dirArgument + ".";
      cmd += _last_matches[0] + ".__doc__";
      PyObject * str = PyRun_String(cmd.toStdString().c_str(), Py_eval_input, _g, _g);
      if (!str || str == Py_None || !PyString_Check(str))
        {
          if (!str)
            PyErr_Clear();
          _doc_str = "";
        }
      else
        _doc_str = QString(PyString_AsString(str));
      Py_XDECREF(str);
    }

  // The command has been successfully executed
  return 0;
}

/**
 * See runDirCommand().
 * @param dirArgument see runDirCommand()
 * @param startMatch  see runDirCommand()
 * @param[out] result the list of matches
 * @param discardSwig if true a regular expression is used to discard all static method generated
 * by SWIG. typically: MEDCouplingUMesh_Blabla
 * @return -1 if the call to dir() or the parsing of the result failed, 0 otherwise.
 */
int PyConsole_EnhInterp::runDirAndExtract(const QString& dirArgument,
       const QString & startMatch, std::vector<QString> & result,
       bool discardSwig) const
{
  QRegExp re("^[A-Z].+_[A-Z]+[a-z]+.+$");  // discard SWIG static method, e.g. MEDCouplingUMesh_Blabla
  QString command("dir(" + dirArgument + ")");
  PyObject * plst = PyRun_String(command.toStdString().c_str(), Py_eval_input, _g, _g);
  if(!plst || plst == Py_None) {
    if(!plst)
      PyErr_Clear();

    Py_XDECREF(plst);
    return -1;
  }

  // Extract the returned list and convert it to a vector<>
  if (!PySequence_Check(plst))
    {
      // Should never happen ...
      //std::cerr << "not a list!" << std::endl;
      Py_XDECREF(plst);
      return -1;
    }

  // Convert plst to a vector of QString
  int n = PySequence_Length(plst);
  for (int i = 0; i < n; i++)
    {
      PyObject * it;
      it = PySequence_GetItem(plst, i);
      QString s(PyString_AsString(it));
      // if the method is not from swig, not static (guessed from the reg exp) and matches
      // what is already there
      if (s.startsWith(startMatch))
        if(!discardSwig || (!re.exactMatch(s) && !s.contains("swig")))
          result.push_back(s);
      Py_DECREF(it);
    }
  Py_DECREF(plst);

  return 0;
}

/**
 * Clear internal members containing the last completion results.
 */
void PyConsole_EnhInterp::clearCompletion()
{
  _last_matches.resize(0);
  _doc_str = QString("");
}



