#include <textreader.h>
#include <taction.h>
#include <tutil.h>

#include <QtDebug>

TextReader::TextReader(QObject *p) : QObject(p), d_configured(false), d_action(0)
{
	connect(TUtil::instance(), SIGNAL(newAction(QString,TAction*)), this, SLOT(configure(QString,TAction*)));
}

TextReader::~TextReader()
{
	unsetSource();
}

void TextReader::setSource(QString s)
{
	TUtil::instance()->subscribe(s);
	d_source = s;
}

void TextReader::unsetSource()
{
	disconnect(this, SLOT(refresh(const TVariant&)));
	TUtil::instance()->unsubscribe(d_source);
	d_source = QString();
}

void TextReader::refresh(const TVariant &v)
{
	qDebug() << QThread::currentThread() << ":" << this << "- refresh:" << d_source << v.toString();
}

void TextReader::read()
{
	TVariant v = d_action->execute();
	refresh(v);
}

void TextReader::configure(QString s, TAction *r)
{
	qDebug() << QThread::currentThread() << ":" << this << "- configure() <<" << s;
	if (d_configured)
		qDebug() << QThread::currentThread() << ":" << this << "- gia` configurato";
	else
	{
		if (d_source == s)
		{
			d_configured = true;
			d_action = r;
			qDebug() << QThread::currentThread() << ":" << this << "- connetto";
			connect(r, SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)), Qt::QueuedConnection);
			disconnect(this, SLOT(configure(QString,TAction*)));
		}
	}
	qDebug() << QThread::currentThread() << ":" << this << "- configure() >>";
}

