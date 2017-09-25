#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTreeWidget;
class QTreeWidgetItem;
class TAppInfo;
class QTimer;
class ActionFactoryInterface;
class DeviceThreadFactoryInterface;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:

    void addApplication(const TAppInfo& app);
    void removeApplication(const TAppInfo& app);

//    void refreshAppsList(bool success, const QString&err);

    void widgetListItemSelectionChanged();
    void appListItemSelectionChanged();
    void slotItemClicked();
    void actionSelectionChanged();
    void propertiesItemClicked();
    void execute();
    void read();
    void le1TextChanged(QString);

    void showError(const QString& message);

    void switchInterface(const QString& type);

    void setupTAppIf(const QTreeWidgetItem *it);
    void setupDevThreadFactoryIf(const QTreeWidgetItem *it);
    void setupActionFactoryIf(const QTreeWidgetItem *it);
    void setSelectedActionAutoRefreshEnabled(bool);
    void setSelectedActionRefreshInterval(int);

    void connectActionFactorySignals(ActionFactoryInterface* currentIface);

    void slotTargetExecuted(const QString &point, int timestamp, bool success, const QString &exception, const QString &data);


private:
    Ui::MainWindow *ui;

    QVariant m_getArg(int pos);

    void m_getArgTypes();

    QStringList m_argTypes;

    QStringList m_supportedTypes;

    TAppInfo selectedItemAppInfo();

    QTimer *m_actionRefreshTimer;
};

#endif // MAINWINDOW_H
