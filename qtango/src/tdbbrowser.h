#ifndef TDBBROWSER_H
#define TDBBROWSER_H

#include <QTreeWidget>
#include <QSize>

#include <QStringList>
#include <qtangocore.h>
#include <eapplicationlauncher.h>

class QMenu;
class QAction;
class TDbBrowserItem;
class TDbBrowserPrivate;
class TDbBrowserItemPrivate;

class TDbBrowser : public QTreeWidget
{
Q_OBJECT
public:
	enum Mode
	{
		ShowNormal	= 0x0000,
		ShowHwDevices	= 0x0001,
		ShowAdmDevices	= 0x0002,
		ShowAll		= ShowHwDevices | ShowAdmDevices
	};
		
	TDbBrowser(QWidget * =0, QString = QString(), int = 0, Mode = ShowNormal);

	void setDatabase(QString, unsigned int);
	QString database();

        int databasePort() { return dbPort; }

	void setMode(Mode m);
	Mode mode() { return d_mode; }

	void setReadOnly(bool b);
	bool isReadOnly(){ return d_readOnly; };

	void setMaxLevel(unsigned int);
	unsigned int maxLevel(){ return d_maxLevel; };

	QStringList selectedAttrCommands();
	QStringList selectedDevices();
	QStringList selectedMembers();
	QStringList selectedFamilies();
	QStringList selectedDomains();

  signals:
	void domainExpanded(QTreeWidgetItem* it);
	void familyExpanded(QTreeWidgetItem* it);
	void memberExpanded(QTreeWidgetItem* it);
	
protected slots:
	void getExpandedDevices(QTreeWidgetItem *);
	void launchApplication(QTreeWidgetItem *, int);
	void changeTangoHost();
	void contextMenuEvent(QContextMenuEvent *event);
	void changeMode();
	
protected:
	Mode		d_mode;

	QString		dbName;
	QString		dbHost;
	unsigned int	dbPort;
	Database	*db;
	DeviceProxy	*dbDevice;

	QMenu		*d_menu;
	QAction		*d_showHwDevices;
	QAction		*d_showAdmDevices;
	QMenu		*d_menu2;

	bool		d_readOnly;
	unsigned int	d_maxLevel;
	
	QMimeData * mimeData ( const QList<QTreeWidgetItem *> items ) const;
	
  private:
    bool d_dbByEnv;

    TDbBrowserPrivate *d_ptr;
};

class TDbBrowserItem : public QObject, public QTreeWidgetItem
{
Q_OBJECT
public:
	TDbBrowserItem(TDbBrowser *, QStringList, QString);
	TDbBrowserItem(TDbBrowserItem *, QStringList, QString);

	void init(QString);

	void searchApplication();

	int 	d_level;
	QString d_pattern;

	QString d_domain;
	QString d_family;
	QString	d_member;
	QString	d_device;
	QString d_attr_comm;
	QString	d_fullName;
	QString d_helperApplication;

	EApplicationLauncher	*app;
	
	bool	isCommand;

	QWidget	*widget;

public slots:
	void launchApplication();
	void copyToClipboard();

private:
    TDbBrowserItemPrivate *d_ptr;
};	

#endif
