#ifndef DBCURVEINTERPOLATOR_H
#define DBCURVEINTERPOLATOR_H

#include <QObject>

class EPlotCurve;

class DbCurveInterpolatorPrivate;

class DbCurveInterpolator : public QObject
{
    Q_OBJECT
public:
    explicit DbCurveInterpolator(EPlotCurve *curve, int period);
    
signals:
    
public slots:
    void interpolate();

private:
    DbCurveInterpolatorPrivate *d_ptr;
    Q_DECLARE_PRIVATE(DbCurveInterpolator)
    
};

#endif // DBCURVEINTERPOLATOR_H
