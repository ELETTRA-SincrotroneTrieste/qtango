#ifndef QTANGOTHDB_H
#define QTANGOTHDB_H

#include <QObject>
#include <QStringList>
#include <QSet>
#include <thdbbase.h>
#include "hdbwidget.h"

class QAction;
class DbPlotAdapterInterface;
class THdbConfigWidget;
class TPlotLightMarker;

class QTangoTHdb : public QObject
{
    Q_OBJECT
public:
    explicit QTangoTHdb(QObject *parent, THdbBase::Type t = THdbBase::Historical);

    virtual ~QTangoTHdb();

    void setupAction();

    QAction *action() { return m_action; }

signals:

public slots:
    void isSourceArchived(const QString&);

    virtual void execute();

    virtual void updateView();

private slots:
    void sourceArchived(const QString&, bool);

    void confDialogDestroyed();

private:
    QAction *m_action;
    QSet<QString> m_sources;
    HdbWidget *m_hdbWidget;
};

#endif // QTANGOTHDB_H
