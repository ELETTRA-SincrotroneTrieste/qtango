/***************************************************************************
 *   Copyright (C) 2007 by Claudio Scafuri, Giacomo Strangolino   *
 *   claudio@hyo   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "showhide.h"
#include <qtango.h>
#include <QSettings>
#include <../../qtangocore/utils/propertyreader.h>
#include <QtDebug>

WShowHide::WShowHide(QWidget *parent) : QWidget(parent)
{
	qApp->setOrganizationName("Elettra");
	qApp->setOrganizationDomain("elettra.trieste.it");
	qApp->setApplicationName("ShowHide");
	ui.setupUi(this);
	ui.labelDoubleScalar->setObjectName(QString("TLabel 1 TAB1 \"%1\"").arg(ui.labelDoubleScalar->realSource()));
	ui.labelShortScalar_ro->setObjectName(QString("TLabel 2 TAB1 \"%1\"").arg(ui.labelShortScalar_ro->realSource()));
	ui.label_tab2_short_scalar->setObjectName(QString("TLabel 1 TAB2 \"%1\"").arg(ui.label_tab2_short_scalar->realSource()));
	ui.label_TAB2_double_scalar->setObjectName(QString("TLabel 2 TAB2 \"%1\"").arg(ui.label_TAB2_double_scalar->realSource()));
	
	connect(ui.labelDoubleScalar->qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(update(const TVariant&)));
	connect(ui.labelShortScalar_ro->qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(update(const TVariant&)));
	connect(ui.label_tab2_short_scalar->qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(update(const TVariant&)));
	connect(ui.label_TAB2_double_scalar->qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(update(const TVariant&)));

	ui.textEdit->setHtml("<html><head></head><body><h3>ShowHide widget</h3>"
			"<p>This widget demonstrates how show/hide events can be intercepted to stop/start qtangocore reading</p>"
			"<h3>How this works</h3>"
			"<p>To appreciate this feature, you must export <br/>"
			"<strong>export QTANGO_PRINT=\"yes\"</strong><br/>"
			"on the console before launching the panel</p>"
			"<p>Check or uncheck the box <cite>Enable Hide Events</cite> to enable/disable this feature. "
			"The choice is remembered via QSettings next time you start the panel.</p></body></html>");

	QSettings s;
	ui.checkBox->setChecked(s.value("SHOW_HIDE_ENABLED", true).toBool());
	enableShowHide(ui.checkBox->isChecked());
	connect(ui.checkBox, SIGNAL(toggled(bool)), this, SLOT(enableShowHide(bool)));

	PropertyReader *pr = new PropertyReader("test/device/1",  this);
	printf("setto blocking in PropertyReader\n");
	pr->setBlocking(true);
	pr->read(QStringList() << "values" << "helperApplication" << "helperApplication2Prop");
//	pr->read("helperApplication");
//	pr->read("helperApplication2Prop");
	printf("read appena chiamata\n");

	printf("vediamo i risultati:\n");
	qDebug() << "values" << pr->propertyList("values");
	qDebug() << "helperApplication" << pr->propertyList("helperApplication");
	qDebug() << "helperApplication2Prop" << pr->propertyList("helperApplication2Prop");
	qDebug() << "values (first)" << pr->property("values");


	printf("- adesso class property: \n");
	pr->setIsDeviceProperty(false);
	pr->setSourceName("TangoTest");
	pr->read("cvs_location");
	qDebug() << "cvs_location prop" << pr->property("cvs_location");
}

void WShowHide::update(const TVariant &v)
{
	QString objectNam;
	QObject *par, *_sender = sender();
	QTangoCommunicationHandle *handle;
	if(qobject_cast<QTangoCommunicationHandle *>(_sender))
		handle = qobject_cast<QTangoCommunicationHandle *>(_sender);
	if(handle)
		par = handle->parent();
	if(par)	
		objectNam = par->objectName();
	if(_sender )
	{
		//printf("refreshed widget \"\e[1;32m%s\e[0m\" with source \"\e[0;32m%s\e[0m\"", qstoc(objectNam), 
		//	v.getAttributeInfo().name.c_str());
		QTangoComProxyReader *reader = dynamic_cast<QTangoComProxyReader *>(par);
	//	if(reader && reader->refreshMode() == POLLED_REFRESH)
	//		printf("[\e[1;36m POLLED \e[0m with period %dms]\n", reader->period());
		if(reader)
			printf("[\e[1;33m %s\e[0m ]\n", qstoc(TUtil::instance()->modeRepresentation(reader->refreshMode())));
	}
	else
		printf("cast error\n");
}

void WShowHide::enableShowHide(bool e)
{
	QSettings s;
	if(e)
		printf("\e[1;32m + \e[0m enabling show hide events on widgets\n");
	else
		printf("\e[1;35m - \e[0m disabling show hide events on widgets\n");

	QList<QWidget *> widgets = findChildren<QWidget *>();
	foreach(QWidget *w, widgets)
	{
		if(dynamic_cast<QTangoWidgetCommon *>(w))
			dynamic_cast<QTangoWidgetCommon *>(w)->setHideEventEnabled(e);
	}
	s.setValue("SHOW_HIDE_ENABLED", e);
}

WShowHide::~WShowHide()
{

}    



