#include "timeforwardbackward.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QDebug>

TimeForwardBackward::TimeForwardBackward(QWidget * parent)
    : QWidget(parent)
{


    QPixmap * pix_forward = new QPixmap(":/images/forward.jpeg");
    QPixmap * pix_backward = new QPixmap(":/images/backward.jpeg");


    QPixmap * pix_wireshark = new QPixmap(":/images/wireshark.png");
    QPixmap * pix_etherape = new QPixmap(":/images/etherape.png");




    forward = new QPushButton(this);
    backward = new QPushButton(this);
    button_wireshark = new QPushButton(this);
    button_etherape = new QPushButton(this);



    forward->setMinimumHeight(100);
    forward->setMinimumWidth(100);
    forward->setIcon(QIcon(*pix_forward));
    forward->setIconSize(QSize(60,60));



    backward->setMinimumHeight(100);
    backward->setMinimumWidth(100);
    backward->setIcon(QIcon(*pix_backward));
    backward->setIconSize(QSize(60,60));



    button_wireshark->setMinimumHeight(40);
    button_wireshark->setMinimumWidth(40);
    button_wireshark->setIcon(QIcon(*pix_wireshark));
    button_wireshark->setIconSize(QSize(40,40));



    button_etherape->setMinimumHeight(40);
    button_etherape->setMinimumWidth(40);
    button_etherape->setIcon(QIcon(*pix_etherape));
    button_etherape->setIconSize(QSize(40,40));



    QHBoxLayout * link_programs = new QHBoxLayout();
    link_programs->addWidget(button_wireshark);
    link_programs->addWidget(button_etherape);



    QVBoxLayout * layout = new QVBoxLayout();
    //QLabel * label = new QLabel("Time selection");




    layout->addLayout(link_programs);
    layout->addWidget(forward);
    layout->addWidget(backward);

    setLayout(layout);


    connect(forward, SIGNAL(clicked()), this, SIGNAL(moveForward()));
    connect(backward, SIGNAL(clicked()), this, SIGNAL(moveBackward()));
    connect(button_etherape, SIGNAL(clicked()), this, SIGNAL(open_etherape()));
    connect(button_wireshark, SIGNAL(clicked()), this, SIGNAL(open_wireshark()));

}

