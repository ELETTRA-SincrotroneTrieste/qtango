#ifndef ACTIONFACTORYOBJECTFILTER_H
#define ACTIONFACTORYOBJECTFILTER_H

#include <QObject>

class ActionFactoryObjectFilter : public QObject
{
Q_OBJECT
public:
    explicit ActionFactoryObjectFilter(QObject *parent = 0);

signals:
    void targetExecuted(const QString&, int, bool, const QString&, const QString&);

protected:
    bool eventFilter(QObject *, QEvent*);

};

#endif // ACTIONFACTORYOBJECTFILTER_H
