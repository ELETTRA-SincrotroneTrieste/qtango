#include "qtangoprojectwizard.h"
#include "ui_qtangoprojectwizard.h"

#include <QSettings>
#include <QMessageBox>
#include <QTimer>
#include <QDir>
#include <QFileDialog>
#include <QCheckBox>
#include <QTreeWidgetItem>
#include <QtDebug>
#include <QDomDocument>
#include <QDomNode>
#include <QItemDelegate>
#include <QComboBox>
#include <QProcess>
#include <math.h>

MyFileInfo::MyFileInfo(const QString &templateFileNam, const QString &newFileNam, const QString &subdirnam)
{
    templateFileName = templateFileNam;
    newFileName = newFileNam;
    subDirName = subdirnam;
}

QTangoProjectWizard::QTangoProjectWizard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTangoProjectWizard)
{
    ui->setupUi(this);
    qApp->setApplicationName("QTangoProjectWizard");
    QTimer::singleShot(200, this, SLOT(init()));
}

QTangoProjectWizard::~QTangoProjectWizard()
{
    delete ui;
}

void QTangoProjectWizard::init()
{
    QSettings s;
    if(s.allKeys().isEmpty())
    {
        QMessageBox::information(this, "Information",
                                 "This seems to be the first time you use the QTango Project Wizard.\n"
                                 "Fill in the form to create your first project. This application will\n"
                                 "remember many of your personal settings.");
    }

    connect(ui->cbLaunch1, SIGNAL(toggled(bool)), ui->leLaunch1, SLOT(setEnabled(bool)));
    connect(ui->cbLaunch2, SIGNAL(toggled(bool)), ui->leLaunch2, SLOT(setEnabled(bool)));

    connect(ui->pbCreate, SIGNAL(clicked()), this, SLOT(create()));
    connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->pbChooseLocation, SIGNAL(clicked()), this, SLOT(selectLocation()));
    connect(ui->pbAddProperty, SIGNAL(clicked()), this, SLOT(addProperty()));
    connect(ui->pbRemoveProperty, SIGNAL(clicked()), this, SLOT(removeProperty()));

    connect(ui->leProjectName, SIGNAL(textChanged(QString)), this, SLOT(projectNameChanged(QString)));
    foreach(QLineEdit *le, findChildren<QLineEdit *>())
    {
        if(le->property("save").isValid() && le->property("save").toBool()
                && s.contains(le->objectName()))
            le->setText(s.value(le->objectName()).toString());
        if(le->property("checkContents").isValid())
            connect(le, SIGNAL(textChanged(QString)), this, SLOT(checkText(QString)));
        /* initialize to invalid the empty line edits with property checkContents to not_empty */
        QStringList nonempty  = QStringList() << "not_empty" << "path" << "project_name";
        if(le->property("checkContents").isValid() &&  nonempty.contains(le->property("checkContents").toString()))
            setValid(le, !le->text().isEmpty() );
    }
    foreach(QRadioButton *r, findChildren<QRadioButton *>())
    {
        if(r->property("save").isValid())
            r->setChecked(s.value(r->objectName()).toBool());
    }
    foreach(QCheckBox *cb, findChildren<QCheckBox *>())
    {
        if(cb->property("save").isValid())
            cb->setChecked(s.value(cb->objectName()).toBool());
    }

    /* finally, group boxes */
    foreach(QGroupBox *gb, findChildren<QGroupBox *>())
    {
        if(gb->property("save").isValid())
            gb->setChecked(s.value(gb->objectName()).toBool());
    }

    QStringList treeItems = s.value("treeItems").toStringList();
    foreach(QString ti, treeItems)
        addProperty(ti.split(";;"));
    checkValidity();

}

void QTangoProjectWizard::create()
{
    QSettings s;

    QString pro_file = ui->leProFile->text();
    QString maincpp = ui->leMain->text();
    QString cpp = ui->leCppFile->text();
    QString h = ui->leCHFile->text();
    QString form = ui->leFormFile->text();
    QString location = ui->leLocation->text();
    QString project_name = ui->leProjectName->text();
    if(!location.endsWith("/"))
        location += "/";
    QString project_path = location + project_name;

    if(maincpp.isEmpty() || cpp.isEmpty() || h.isEmpty() || form.isEmpty() || pro_file.isEmpty()
            || location.isEmpty() || project_name.isEmpty())
    {
        printf("\e[1;31m* \e[0mSome compulsory fields are empty!\n");
        return;
    }

    foreach(QLineEdit *le, findChildren<QLineEdit *>())
    {
        if(le->property("save").isValid())
            s.setValue(le->objectName(), le->text());
    }
    foreach(QRadioButton *r, findChildren<QRadioButton *>())
    {
        if(r->property("save").isValid())
            s.setValue(r->objectName(), r->isChecked());
    }
    foreach(QCheckBox *cb, findChildren<QCheckBox *>())
    {
        if(cb->property("save").isValid())
            s.setValue(cb->objectName(), cb->isChecked());
    }

    /* finally, group boxes */
    foreach(QGroupBox *gb, findChildren<QGroupBox *>())
    {
        if(gb->property("save").isValid())
            s.setValue(gb->objectName(), gb->isChecked());
    }

    QStringList treeItems;
    foreach(QTreeWidgetItem *it, ui->twProperties->findItems("*", Qt::MatchWildcard))
        if(it->text(0) != "-" && it->text(1) != "-")
            treeItems << it->text(0) + ";;" + it->text(1)  + ";;" +
                         qobject_cast<QComboBox *>(ui->twProperties->itemWidget(it, 2))->currentText();
    s.setValue("treeItems" , treeItems);

    QDir newDir(project_path);
    if(newDir.exists() && !newDir.entryList().isEmpty())
    {
        QMessageBox::StandardButton  proceed = QMessageBox::question(this, "Directory exists", "The directory \n\""
                                                                     + newDir.path() + "\"\nAlready exists and is not empty.\n"
                                                                                       "Do you want to create the new files there?",
                                                                     QMessageBox::Yes|QMessageBox::Cancel);
        if(proceed != QMessageBox::Yes)
            return;
    }

    if(newDir.mkpath(project_path)) /* returns true if dir already exists */
    {
        QString wtype, subdirname, outdirname, outfilename;
        QDir subdir;
        bool subdirOk = true;
        ui->rbWidget->isChecked() ? wtype = "widget" : wtype = "mainwindow";
        QString template_path = TEMPLATES_PATH;
        if(!template_path.endsWith('/'))
            template_path += '/';

        QList<MyFileInfo> filesInfo;
        MyFileInfo proi("qtangoproject.pro", pro_file, "");
        MyFileInfo maini("main.cpp", maincpp, "src");
        MyFileInfo cppi(wtype + ".cpp", cpp, "src");
        MyFileInfo hi (wtype + ".h", h, "src");
        MyFileInfo formi(wtype + ".ui", form, "src");
        MyFileInfo readmei("README", "README", "");
        MyFileInfo changelogi("CHANGELOG", "CHANGELOG", "");

        filesInfo << proi << maini << cppi << hi << formi << readmei << changelogi;

        newDir.setPath(project_path);

        foreach(MyFileInfo fi, filesInfo)
        {
            QString fname = fi.templateFileName;
            QString placeh_lower;
            QFile f(template_path + fname);
            qDebug() << __FUNCTION__ << "reading from file template " << fname;
            QString contents;
            if(f.open(QIODevice::ReadOnly|QIODevice::Text))

            {
                contents = QString(f.readAll());
                foreach(QLineEdit *le, findChildren<QLineEdit *>())
                {
                    if(le->isEnabled() && !le->text().isEmpty())
                    {
                        QVariant placeholder = le->property("placeholder");
                        if(placeholder.isValid())
                        {
                            contents.replace(placeholder.toString(), le->text());
                            /* look for _LOWERCASE in contents */
                            placeh_lower = placeholder.toString();
                            placeh_lower.remove(placeh_lower.length() - 1, 1) + "_LOWERCASE$";
                            contents.replace(placeh_lower, le->text().toLower());
                        }
                    }
                }
                f.close(); /* done with the file */

                /* replace unmatched place holders with "-" */
                contents.replace(QRegExp("\\$[A-Za-z_0-9]*\\$"), "-");

                if(fname.endsWith(".ui"))
                    addProperties(contents);

                subdirname = fi.subDirName;
                outdirname = newDir.absolutePath() + "/" + subdirname;
                qDebug() << __FUNCTION__ << " out dir name " << outdirname;
                if(!outdirname.endsWith('/'))
                    outdirname += "/";
                outfilename = outdirname + fi.newFileName;

                if(!subdirname.isEmpty())
                    subdir = QDir(newDir.absolutePath() + "/" + subdirname);
                if(!subdir.exists())
                    subdirOk = newDir.mkdir(subdirname);
                if(!subdirOk)
                {
                    QMessageBox::critical(this, "Error creating directory", "Error creating subdirectory \'" + subdirname + "\"\n"
                                               "under \"" + newDir.path() + "\"");
                }
                else
                {
                    printf("* \e[1;36mwill output on file \"%s\"\e[0m\n", outfilename.toStdString().c_str());

                    printf("================================================\n\e[0;37m%s\e[0m\n\n", contents.toStdString().c_str());

                    QFile out(outfilename, this);
                    if(!out.open(QIODevice::WriteOnly|QIODevice::Text))
                    {
                        QMessageBox::critical(this, "Failed to open file for writing",
                                              QString("Failed to open \"%1\" for writing:\n%2"
                                                      "\nFile generation will be incomplete.").arg(outfilename).arg(out.errorString()));
                        break;
                    }
                    else
                    {
                        QTextStream fo(&out);
                        fo << contents;
                        f.close();
                    }

                }
            }
            else
            {
                QMessageBox::critical(this, "Failed to create file",
                                      "Error reading template file \"" + f.fileName() +
                                      "\"\nProject creation will be incomplete!\n"
                                      "The error was: \"" + f.errorString() + "\"");
                break;
            }
        }

        QList<QLineEdit *> lesCmd = QList<QLineEdit *>() << ui->leLaunch1 << ui->leLaunch2;
        foreach(QLineEdit *le, lesCmd)
        {
            if(le->isEnabled() && !le->text().isEmpty())
            {
                QStringList cmdline = le->text().split(QRegExp("\\s+"), QString::SkipEmptyParts);
                QStringList args(cmdline);

                if(cmdline.size() > 1)
                {
                    args.removeFirst();
                    for(int i = 0; i < args.size(); i++)
                    {
                        args[i].replace("$PRO_FILE", proi.newFileName);
                        args[i].replace("$UI_FILE",  formi.subDirName + "/" + formi.newFileName);
                    }
                }
                printf("\e[1;32m Launching le process %s\n", cmdline.join(" ").toStdString().c_str());
                QProcess process(this);
                int res = process.startDetached(cmdline.first(), args, project_path);
                if(!res)
                    QMessageBox::critical(this, "Application launch failed",
                                          QString("Failed to launch %1:\n%2").arg(cmdline.join(" "))
                                          .arg(process.errorString()));
            }
        }

        printf("\e[1;32m*\n* \e[0mcopy and paste the following line to change into the project directory:\n\e[1;32m*\e[0m\n");
        printf("\e[1;32mcd %s\e[0m\n", project_path.toStdString().c_str());
        if(ui->cbCloseAfterCreate->isChecked())
            close();
    }
    else
        QMessageBox::critical(this, "Failed to create directory",
                              QString("Failed to create directory \n\"%1\"").arg(newDir.path()));
}

void QTangoProjectWizard::checkText(const QString &txt)
{
    bool valid = true;
    QString checkType = sender()->property("checkContents").toString();
    if(checkType == "project_name")
        valid = !txt.isEmpty() && !txt.contains(QRegExp("\\s+"));
    else if(checkType == "not_empty")
        valid = !txt.isEmpty();
    else if(checkType == "email")
        valid = txt.isEmpty() || txt.count("@") == 1;
    else if(checkType == "path")
    {
        QDir d(txt);
        valid = d.exists();
    }

    setValid(qobject_cast<QWidget *>(sender()), valid);

    checkValidity();
}

void QTangoProjectWizard::selectLocation()
{
    QString startDirName = ui->leLocation->text();
    QDir startDir(startDirName);
    if(!startDir.exists())
        startDirName = QDir::homePath();
    QString path = QFileDialog::getExistingDirectory(this, "Choose Directory", startDirName,
                                                     QFileDialog::ShowDirsOnly);
    if(!path.isEmpty())
        ui->leLocation->setText(path);

}

void QTangoProjectWizard::addProperty(const QStringList parts)
{
    Q_ASSERT(parts.size() == 3);
    QTreeWidgetItem *it = new QTreeWidgetItem(ui->twProperties, QStringList() << parts.at(0) << parts.at(1) << parts.at(2));
    it->setFlags(it->flags() | Qt::ItemIsEditable);
    QComboBox *propDataTypeCombo = new QComboBox(ui->twProperties);
    propDataTypeCombo->addItems(QStringList() << "bool" << "string" << "int" << "float" << "double");
    propDataTypeCombo->setCurrentIndex(propDataTypeCombo->findText(parts.at(2)));
    ui->twProperties->setItemWidget(it, 2, propDataTypeCombo);
    ui->twProperties->editItem(it, 0);
}

void QTangoProjectWizard::removeProperty()
{
    foreach(QTreeWidgetItem *it, ui->twProperties->selectedItems())
        delete it;
}

void QTangoProjectWizard::projectNameChanged(const QString &p)
{
    QList<QLineEdit *> les = ui->grBoxFiles->findChildren<QLineEdit *>();
    /* add application name */
    les << ui->leApplicationName;
    foreach(QLineEdit *le, les)
    {
        QString s = le->text();
        QString p1(p), s1(s);
        QString extension = le->property("extension").toString();
        s1.remove(extension);

        if(fabs(s1.length() - p.length()) < 2)
        {
            p1.truncate(qMin(s1.length(), p.length()));
            s1.truncate(qMin(s1.length(), p.length()));
            if(s1.compare(p1, Qt::CaseInsensitive) == 0)
                s = p + extension;
            if(s.length() > 0 && extension.isEmpty())
                s[0] = s[0].toUpper();

            le->setText(s);
        }
    }

}

void QTangoProjectWizard::checkValidity()
{
    bool valid = true;
    foreach(QLineEdit *le, findChildren<QLineEdit *>())
    {
        if(le->property("valid").isValid() && le->property("valid").toBool() == false)
            valid = false;
        if(!valid)
            break;
    }
    ui->pbCreate->setEnabled(valid);
}

void QTangoProjectWizard::setValid(QWidget *w, bool valid)
{
    /* take the palette from a line edit whose palette is never changed */
    QPalette p = ui->leAuthorPhone->palette();
    if(!valid)
        p.setColor(QPalette::Text, QColor(Qt::red));
    w->setPalette(p);
    w->setProperty("valid", valid);
}

void QTangoProjectWizard::loadSettings()
{

}

void QTangoProjectWizard::addProperties(QString &uixml)
{
    if(ui->twProperties->model()->rowCount() > 0)
    {
        QDomDocument doc("uixml");
        if(doc.setContent(uixml))
        {
            QDomElement widget;
            QDomElement uiel = doc.firstChildElement("ui");
            if(!uiel.isNull())
                widget = uiel.firstChildElement("widget");
            if(!widget.isNull())
            {
                foreach(QTreeWidgetItem *it, ui->twProperties->findItems("*", Qt::MatchWildcard))
                {
                    QString pName = it->text(0);
                    QString pVal  = it->text(1);
                    QString type = qobject_cast<QComboBox *>(ui->twProperties->itemWidget(it, 2))->currentText();
                    if(pName != "-" && pVal != "-")
                    {
                        QDomElement propEl = doc.createElement("property");
                        propEl.setAttribute("name", pName);
                        propEl.setAttribute("stdset", "0");
                        widget.appendChild(propEl);
                        QDomElement valElement = doc.createElement(type);
                        valElement.appendChild(doc.createTextNode(pVal));
                        propEl.appendChild(valElement);
                    }
                }
            }
            else
                qDebug() << __FUNCTION__ << "widget el is null!!";
            uixml = doc.toString();
            qDebug() << __FUNCTION__ << uixml;
        }
        else
        {
            QMessageBox::critical(this, "Error parsing ui xml file!", "Error parsing ui file!");
        }
    }
}

