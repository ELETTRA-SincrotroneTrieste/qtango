#ifndef TUNIQUEAPPLICATION_H
#define TUNIQUEAPPLICATION_H

#include <TApplication>

class TUniqueApplication : public TApplication
{
    Q_OBJECT
public:
    TUniqueApplication( int & argc, char ** argv );

    int exec();
};

#endif // TUNIQUEAPPLICATION_H
