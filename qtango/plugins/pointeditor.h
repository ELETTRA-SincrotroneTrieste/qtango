#ifndef POINT_EDITOR_H
#define POINT_EDITOR_H

#include <QDialog>

class QComboBox;
class QLineEdit;
class QCheckBox;

#include <QLabel>

class PointEditor : public QDialog
{
  Q_OBJECT
  public:
	PointEditor(QWidget *parent, const QString &defaultTxt);
	
	QString point() { return d_point; }

        QLabel *textLabel() const { return findChild<QLabel *>("textLabel"); }
	
  protected slots:
	void accept(); /* reimplemented */
	void commandSwitch(bool);
	void comboChanged(int);
	
  private:
	QComboBox *cb;
	QLineEdit *le;
	QCheckBox *cbox;
	QString d_point, d_savedPoint;
};


#endif
