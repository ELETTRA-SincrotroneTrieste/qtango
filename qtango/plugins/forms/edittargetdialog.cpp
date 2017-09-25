/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "edittargetdialog.h"

// #include <QtDesigner>

// using namespace qdesigner_internal;

EditTargetDialog::EditTargetDialog(QWidget *parent) : EStringListEditor(parent),
  browserDialog(0)
{
  ui.tdbBrowserButton->setVisible(true);
  connect(ui.tdbBrowserButton, SIGNAL(clicked()), this, SLOT(openBrowser()));
  ui.groupBox->setTitle("Edit Tango Targets");
  setWindowTitle("Edit tango targets");
}

void EditTargetDialog::openBrowser()
{
	if (browserDialog == 0)
	{
	  browserDialog = new QDialog(this);
	  browserUi.setupUi(browserDialog);
	  browserUi.treeBrowser->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	if (browserDialog->exec() == QDialog::Accepted)
	{
	  QStringList devs = browserUi.treeBrowser->selectedAttrCommands();
	  if (devs.size())
	  {
		QString target;
		if (browserUi.checkParametric->isChecked())
		{
		  target = "$" + QString::number(browserUi.spinDevice->value());
		  if (devs[0].contains("->"))
				  target += "->" + devs[0].split("->").last();
		  else
				  target += "/" + devs[0].split("/").last();
		}
		else
				target = devs[0];
		if (browserUi.checkDatabase->isChecked())
				target.prepend(browserUi.treeBrowser->database() + "/");
		ui.itemTextLineEdit->setText(target);
	  }
	}
}


