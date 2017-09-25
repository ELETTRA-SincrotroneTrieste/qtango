#ifndef TLOGBUTTON_H
#define TLOGBUTTON_H

#include <qtangocore.h>
#include <QPushButton>

class TLogButtonPrivate;

class TLogButton : public QPushButton
{
Q_OBJECT
public:
	TLogButton(QWidget* parent = 0);
	~TLogButton();

public slots:
	void showLogs();

private:
    TLogButtonPrivate *d_ptr;
};
#endif
