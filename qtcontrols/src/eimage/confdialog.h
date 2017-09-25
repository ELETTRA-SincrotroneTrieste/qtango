#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include <QDialog>
#include <QVector>
#include <QRgb>

namespace Ui {
class ConfDialog;
}

class ConfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfDialog(QWidget *parent = 0);
    ~ConfDialog();

    QString getColorTableName() const;

private:
    Ui::ConfDialog *ui;

};

#endif // CONFIGDIALOG_H
