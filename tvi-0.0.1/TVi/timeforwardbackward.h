#ifndef TIMEFORWARDBACKWARD_H
#define TIMEFORWARDBACKWARD_H


#include <QPushButton>


class TimeForwardBackward: public QWidget
{
    Q_OBJECT

public:
    TimeForwardBackward(QWidget *parent = 0);

signals:
    void moveForward();
    void moveBackward();
    void open_wireshark();
    void open_etherape();

private:
    QPushButton * forward;
    QPushButton * backward;


    QPushButton * button_wireshark;
    QPushButton * button_etherape;


};

#endif // TIMEFORWARDBACKWARD_H
