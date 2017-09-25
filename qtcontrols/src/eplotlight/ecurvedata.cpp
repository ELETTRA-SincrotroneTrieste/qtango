#include "ecurvedata.h"

CurveData::CurveData():
    d_count(0)
{
  
}

void CurveData::removeFirstElements(int n)
{
  if(d_count >= n)
  {
//     printf("rimuovo primi %d elementi x[0]: %f e y[0]: %f\n", n, d_x[0], d_y[0]);
    d_x.remove(0, n);
    d_y.remove(0, n);
    d_count -= n;
  }
}

void CurveData::set(const QVector< double > &xData, const QVector< double > &yData)
{
  d_x = xData;
  d_y = yData;
  d_count = xData.size();
}

void CurveData::append(double *x, double *y, int count)
{
    int newSize = ( (d_count + count) / 10 + 1 ) * 10;
    if ( newSize > size() )
    {
        d_x.resize(newSize);
        d_y.resize(newSize);
    }

    for ( register int i = 0; i < count; i++ )
    {
        d_x[d_count + i] = x[i];
        d_y[d_count + i] = y[i];
    }
    d_count += count;
}

int CurveData::count() const
{
    return d_count;
}

int CurveData::size() const
{
    return d_x.size();
}

const double *CurveData::x() const
{
    return d_x.data();
}

const double *CurveData::y() const
{
    return d_y.data();
}







