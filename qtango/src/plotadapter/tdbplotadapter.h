#ifndef TDBPLOTADAPTER_H
#define TDBPLOTADAPTER_H

#include "dbplotadapterinterface.h"

class TdbPlotAdapter : public DbPlotAdapterInterface
{
public:
    TdbPlotAdapter(EPlotLight *parent);

    virtual EPlotCurve *getCurve(const QString& source);

    virtual QString curveName(const QString& source);

    virtual QList<EPlotCurve *> historicalCurvesList();

    virtual void uniformizeCurveStyles();

    virtual QString type() const { return "Tdb"; }
};

#endif // HDBPLOTADAPTER_H
