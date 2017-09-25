#ifndef QTANGOPROJECTWIZARD_H
#define QTANGOPROJECTWIZARD_H

#include <QWidget>

namespace Ui {
class QTangoProjectWizard;
}

class MyFileInfo
{
public:
    MyFileInfo(const QString &templateFileNam, const QString& newFileNam, const QString& subdirnam);

    QString templateFileName, newFileName, subDirName;
};

class QTangoProjectWizard : public QWidget
{
    Q_OBJECT

public:
    explicit QTangoProjectWizard(QWidget *parent = 0);
    ~QTangoProjectWizard();

private slots:
    void init();

    void create();

    void checkText(const QString &);

    void selectLocation();

    void addProperty(const QStringList parts = QStringList() << "-" << "-" << "string");

    void removeProperty();

    void projectNameChanged(const QString &);

private:
    void checkValidity();

    void setValid(QWidget *w, bool valid);

    void loadSettings();

    void addProperties(QString &uixml);

private:
    Ui::QTangoProjectWizard *ui;
};

#endif // QTANGOPROJECTWIZARD_H
