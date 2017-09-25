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


#include "timings.h"
#include "timingwidget.h"
#include <QGridLayout>
#include <QCheckBox>
#include <tplotlight_marker.h>
#include <QTimer>
#include <QLabel>
#include <QSpinBox>
#include <QDateTime>
#include <tutil.h>
#include <proc/readproc.h> 
#include <asm/param.h> /* HZ */
#include <elettracolors.h>

#define CPU_REFRESH_INTERVAL 1000

CpuData::CpuData()
{
	oldUtime = oldStime = utime = stime = 0;
}

float CpuData::percentage()
{
	float perc = 0.0;
	unsigned long long delta_jiffies, delta_utime, delta_stime;
	/* CPU_REFRESH_INTERVAL is in milliseconds, so multiply by 1000 */
	delta_jiffies = CPU_REFRESH_INTERVAL / 1000 * HZ;
	proc_t procin, *proc;
     	proc = get_proc_stats(getpid(), &procin);
	/* first time ? */
	if(oldUtime == 0 || oldStime == 0)
	{
		oldUtime = proc->utime;
		oldStime = proc->stime;
	}
	else
	{
		delta_utime = proc->utime - oldUtime;
		delta_stime = proc->stime - oldStime;
		oldUtime = proc->utime;
		oldStime = proc->stime;
		perc = (float) (100 * (delta_utime + delta_stime)) / (float) delta_jiffies;
	}
	
	return perc;
}

Timings::Timings(QWidget *parent) : QWidget(parent)
{
	int i;
	QGridLayout *lo = new QGridLayout(this);
	lo->setSpacing(1);
	lo->setMargin(1);
	printf("\e[4;minformation\e[0m \e[1;32mYou need three TangoTest instances running...\e[0m\n");
	QStringList attributes = QStringList() << "double_scalar" << "short_scalar" << "long_scalar" << "long_scalar_rww" << 
		"short_scalar_ro" << "float_scalar" << "double_scalar_rww";
	/* 3 devices, attributes.size() attributes to display */	
	for(i = 0; i < attributes.size() * 3; i++)
	{
		TimingWidget *tw = new TimingWidget(this);
		tw->setSource(QString("$%1/%2").arg(i % 3 + 1).arg(attributes.at(i  % attributes.size() )));
		lo->addWidget(tw, i, 0, 1, 6);	
	}
	/* global refresh */
	QCheckBox *cb = new QCheckBox(this);
	cb->setText("Global refresh enabled");
	cb->setToolTip("Enabling global refresh leaves the single reader to decide\nto refresh synchronously or asynchronously\n"
		"depending on the single reader.\nDisabling it means disable it globally.");
	cb->setChecked(!TUtil::instance()->globalRefreshDisabled());
	connect(cb, SIGNAL(toggled(bool)), this, SLOT(globalRefreshEnabled(bool)));
	lo->addWidget(cb, i, 0, 1, 3);
	/* global refresh interval */
	QLabel *sbLabel = new QLabel(this);
	sbLabel->setText("Change global refresh interval");
	QSpinBox *sb = new QSpinBox(this);
	sb->setMinimum(10);
	sb->setMaximum(5000);
	sb->setValue(1000);
	sb->setSingleStep(100);
	lo->addWidget(sb, i, 5, 1, 1);
	lo->addWidget(sbLabel, i, 3, 1, 2);
	connect(sb, SIGNAL(valueChanged(int)), TUtil::instance(), SLOT(setGlobalRefreshInterval(int)));
	/* CPU plot */
	d_cpuPlot = new TPlotLightMarker(this);
	lo->addWidget(d_cpuPlot, 0, 6, attributes.size() * 3, 5);
	EPlotCurve *cpuCurve = new EPlotCurve(this, "cpuCurve");
	d_cpuPlot->addCurve("curve", cpuCurve);
	d_cpuPlot->setDataBufferSize(100);	
	cpuCurve->setPen(QPen(KDARKBLUE));	
	/* this disables x axis autoscale for a better refresh of the x axis */
	QDateTime dt = QDateTime::currentDateTime();
	d_cpuPlot->setAxisScale(QwtPlot::xBottom,  dt.toTime_t(), dt.addSecs(CPU_REFRESH_INTERVAL/1000 * 20).toTime_t());	
	d_cpuPlot->setAxisAutoScale(QwtPlot::yLeft);
	d_cpuPlot->setTimeScaleDrawEnabled(true);
	d_cpuPlot->setAxisTitle(	QwtPlot::xBottom,  "time");
	d_cpuPlot->setAxisTitle(	QwtPlot::yLeft,  "CPU usage [%]");
		
	/* cpu plot timer */
	QTimer *timer = new QTimer(this);
	timer->setInterval(CPU_REFRESH_INTERVAL);
	timer->setSingleShot(false);
	connect(timer, SIGNAL(timeout()), this, SLOT(refreshCpu()));
	timer->start();
}


Timings::~Timings()
{

}    

void Timings::globalRefreshEnabled(bool en)
{
	TUtil::instance()->setGlobalRefreshDisabled(!en);
}

void Timings::refreshCpu()
{
		float percentage = d_cpuData.percentage();
		time_t tp;
		time(&tp);
		d_cpuPlot->appendData("curve", (double) tp, (double) percentage);
		d_cpuPlot->refresh();
}

