#ifndef DBPLOTADAPTERINTERFACEPRIVATE_H
#define DBPLOTADAPTERINTERFACEPRIVATE_H

class EPlotLight;
class DbPlotBackground;
class DbPlotAdapterInterface;
class EPlotCurve;

#include <QVector>
#include <QHash>

class DbPlotAdapterInterfacePrivate
{
public:
    DbPlotAdapterInterfacePrivate(DbPlotAdapterInterface* dbpaif, EPlotLight *parent);

    EPlotLight *plot() { return m_plot; }

    virtual void setData(const QString& source, const QVector<double >&xData, const QVector<double> &yData);

    virtual void setSetPointData(const QString& source, const QVector<double >&xData, const QVector<double> &yData);

    void clearHistoricalData();

    void addBackground(double start, double stop);

    DbPlotBackground *background() { return m_background; }

    bool backgroundEnabled () const { return m_background != NULL; }

    void setBackgroundEnabled(bool en);

    void clearCurveHistory(const QString& source);

    void deleteHistoricalCurve(const QString& source);

    void deleteHistoricalCurves();

    QHash<QString, int> interpolatorDataHash;

private:

    DbPlotAdapterInterface* const q_ptr;

    EPlotLight *m_plot;

    DbPlotBackground *m_background;

    Q_DECLARE_PUBLIC(DbPlotAdapterInterface);
};

#endif // DBPLOTADAPTERINTERFACEPRIVATE_H
