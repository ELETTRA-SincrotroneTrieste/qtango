#ifndef READLABEL_H
#define READLABEL_H

#include <QLabel>
#include <com_proxy_reader.h>

class TVariant;

class ReadLabel : public QLabel, public QTangoComProxyReader
{
    Q_OBJECT
public:
    explicit ReadLabel(QWidget *parent = 0);

signals:
    void readOk(bool);
    void message(const QString&);

public slots:

protected slots:

    void refresh(const TVariant &v);

};

#endif // READLABEL_H
