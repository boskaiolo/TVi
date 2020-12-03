#ifndef TIMESTAMP_SELECTED_H
#define TIMESTAMP_SELECTED_H

#include <QObject>
#include <QDebug>

class timestamp_selected : public QObject
{
    Q_OBJECT

public:
    timestamp_selected(){}

    void get_timestamps(long * a, long *b)
    {
        *a = init;
        *b = end;
    }


public slots:
    void set_new_timestamp(long a, long b)
    {
        this->init = a;
        this->end = b;
        qWarning() << "Timestamp in Object timestamp_selected set to: " << init << end;
    }


private:
    long int init;
    long int end;


};

#endif // TIMESTAMP_SELECTED_H
