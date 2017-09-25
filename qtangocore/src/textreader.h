#include <QObject>

class TAction;
class TVariant;

class TextReader : public QObject
{
Q_OBJECT

public:
	TextReader(QObject * = NULL);
	~TextReader();

	void setSource(QString);
	void unsetSource();

public slots:
	virtual void refresh(const TVariant &);
	void configure(QString, TAction*);
	void read();

	QString getData() { return QString::number(111); };

protected:
	bool	d_configured;
	QString d_source;
	TAction	*d_action;
};

