#ifndef ModuleBase_Definitions_H
#define ModuleBase_Definitions_H

#include <QList>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

typedef QList<int> QIntList;       //!< list of int values
typedef QList<short> QShortList;     //!< list of short int values
typedef QList<double> QDoubleList;    //!< list of double values
typedef QList<FeaturePtr> QFeatureList;  //!< List of features
typedef QList<ResultPtr> QResultList;  //!< List of results

#endif
