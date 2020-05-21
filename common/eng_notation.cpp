#include "eng_notation.h"
#include <stdio.h>
#include <math.h>
#include <QRegExp>
#include <QStringList>
#include <macros.h>

EngString::EngString(QString s , const QString& format, const QVariant &value) : QString(s)
{
  suffixes << "y" << "z" << "a" << "f" << "p" << "n" << MICRO << "m" << "" <<
	"k" << "M" << "G" << "T" << "P" << "E" << "Z" << "Y";
  
//   printf("\e[1;33mprefix size : %d sizeof(char *) %d\n", sizeof(prefix), sizeof(char *));
  /* Smallest power of then for which there is a prefix defined.
   * If the set of prefixes will be extended, change this constant
   * and update the table "prefix".
   */
  d_prefixStart = -24;
  d_prefixEnd = (suffixes.size() -1 ) * 3;
  int expof10;
  
  /* check if the format indicates that we want an engineering notation */
  if(format.contains(ENGFMT))
  {
	d_significantDigits = extractSignificantDigits(format);
	int digits = d_significantDigits;
	bool ok;
	double d = value.toDouble(&ok);
// 	printf("value to double %f - significant digits: %d\n", d, digits);
	if(ok && d != 0)
	{
	  double absVal = fabs(d);
	  expof10 = (int) log10(absVal);
      if(expof10 > 0)
        expof10 = (expof10 / 3) * 3;
      else
        expof10 = (-expof10 +3 ) /3 * (-3); 
 
      absVal *= pow(10,-expof10);
	  
	   if (absVal >= 1000.)
         { absVal /= 1000.0; expof10 += 3; }
      else if(absVal >= 100.0)
         digits -= 2;
      else if(absVal >= 10.0)
         digits -= 1;
// 	  printf("looking for suffix at pos %d (value to double %f)\n", (expof10 - d_prefixStart)/3, d);
	  if(/*numeric || */(expof10 < d_prefixStart) ||    
                    (expof10 > d_prefixEnd))
        sprintf("%.*fe%d", digits - 1, absVal, expof10); 
      else
        sprintf("%.*f %s", digits - 1, absVal, 
          qstoc(suffixes[(expof10 - d_prefixStart)/3]));
	  if(d < 0.0)
		prepend("-");
	}
	else if(ok && d == 0)
	{
	  QString format;
	  digits > 0 ? format = QString("%.%1f").arg(digits - 1) : format = "%.1f";
	  sprintf(qstoc(format), 0.0);
	}
  }
  else if(format.isEmpty())
  {
	bool ok;
	pwarn("EngString format is empty to represent variant \"%s\"", qstoc(value.toString()));
	if(value.toDouble(&ok) && ok)
	  setNum(value.toDouble());
	else
	{
	  clear();
	  append(value.toString());
	}
  }

//   else
// 	printf("format\e[1;36m \"%s\"\e[0m does not contain \e[1;32meng\e[0m\n", qstoc(format));
}

int EngString::extractSignificantDigits(const QString &fmt)
{
  int sigDigits = -1;
  if(fmt.contains(ENGFMT))
  {
	/* not escaped pattern %(\d+)(?:.{0,1}\d*)eng */
	QString pattern = QString("%(\\d+)(?:.{0,1}\\d*)%1").arg(ENGFMT);
	QRegExp re(pattern);
	int pos = re.indexIn(fmt);
// 	printf("looking for pattern \"%s\" in \"%s\" : pos %d\n", qstoc(pattern), qstoc(fmt), pos);
	if(pos > -1)
	{
	  QStringList captures = re.capturedTexts();
	  if(captures.size() > 1)
		sigDigits = captures.at(1).toInt();
	}
	else
	  sigDigits = 0; /* no significant digits provided */
  }
  return sigDigits;
}



