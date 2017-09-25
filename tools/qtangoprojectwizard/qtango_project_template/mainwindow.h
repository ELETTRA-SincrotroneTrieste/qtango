#ifndef $MAINCLASS$_H
#define $MAINCLASS$_H

#include <QMainWindow>

namespace Ui {
class $MAINCLASS$;
}

class $MAINCLASS$ : public QMainWindow
{
    Q_OBJECT

public:
    explicit $MAINCLASS$(QWidget *parent = 0);
    ~$MAINCLASS$();

private:
    Ui::$MAINCLASS$ *ui;
};

#endif // $MAINCLASS$_H
