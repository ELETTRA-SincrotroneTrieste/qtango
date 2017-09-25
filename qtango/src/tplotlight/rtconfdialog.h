#ifndef RTCONFDIALOG_H
#define RTCONFDIALOG_H

#include <QDialog>
#include "rtconfwidget.h"
#include <eplot_configuration_widget.h>

class RtConfDialogPrivate;

class RtConfDialog : public QDialog
{
  Q_OBJECT
  public:
	RtConfDialog(QWidget *parent);

private:
    RtConfDialogPrivate *d_ptr;

};

#endif
