#include "rtconfdialog.h"
#include <QTabWidget>
#include <QHBoxLayout>


RtConfDialog::RtConfDialog(QWidget *parent) : QDialog(parent)
{  
  setAttribute(Qt::WA_DeleteOnClose);
  QHBoxLayout *lo = new QHBoxLayout(this);
  QTabWidget *tabWidget = new QTabWidget(this);
  /* QTabWidget doc: Create a QWidget for each of the pages in the tab dialog, but do not specify parent widgets for them. 
   * But EPlotConfigurationWidget needs a plot as parent.. 
   */
  EPlotConfigurationWidget *epConf = new EPlotConfigurationWidget(parent);
  RtConfWidget *rtConf = new RtConfWidget(parent);
  
  lo->addWidget(tabWidget);
  
  tabWidget->addTab(rtConf, "Real Time Configuration");
  tabWidget->addTab(epConf, "Plot Configuration");
}



