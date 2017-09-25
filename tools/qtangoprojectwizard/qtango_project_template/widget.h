#ifndef $MAINCLASS$_H
#define $MAINCLASS$_H

#include <QWidget>

namespace Ui {
class $MAINCLASS$;
}

class $MAINCLASS$ : public QWidget
{
    Q_OBJECT

public:
    explicit $MAINCLASS$(QWidget *parent = 0);
    ~$MAINCLASS$();

private:
    Ui::$MAINCLASS$ *ui;
};

#endif // $MAINCLASS$_H
