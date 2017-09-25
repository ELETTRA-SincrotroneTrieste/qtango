#ifndef TWIDGET_H
#define TWIDGET_H

#include <QWidget>

class TWidgetPrivate;

class TWDropEventFilter : public QObject
{
    Q_OBJECT
public:
    TWDropEventFilter(QObject *parent);

    bool eventFilter(QObject *o, QEvent *e);

};


/** \brief Class used by Qt designer
 *
 */
class TWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TWidget(QWidget *parent = 0);

    virtual ~TWidget();

signals:

public slots:
    void setDesignerMode(bool d);

private:
    TWidgetPrivate *d;
};

#endif // TWIDGET_H
