#include "graphfeatureselector.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>


QRadioButton * create_qradiobutton (QString name) {

    QRadioButton * rb = new QRadioButton (name);
    rb->setFont(QFont("Times", 14, QFont::Bold));

    return rb;
}



void GraphFeatureSelector::selected_protocol(bool tcp, bool udp, bool icmp)
{
    if (tcp == true)
        tcp_sip_dip->setChecked(true);
    else if (udp == true)
        udp_sip_dip->setChecked(true);
    else if (icmp == true)
        icmp_sip_dip->setChecked(true);
}


void GraphFeatureSelector::icmp_selected(bool is_selected)
{
    if (is_selected == true)
    {
        filter_port_begin->setDisabled(true);
        filter_port_end->setDisabled(true);
    }
    else
    {
        filter_port_begin->setEnabled(true);
        filter_port_end->setEnabled(true);
    }
}

GraphFeatureSelector::GraphFeatureSelector(QWidget * parent)
    : QWidget(parent)
{

    tcp_sip_dip = create_qradiobutton("TCP SIP->DIP");
    udp_sip_dip = create_qradiobutton("UDP SIP->DIP");
    icmp_sip_dip = create_qradiobutton("ICMP SIP->DIP");



    QLabel * filter_port_label = new QLabel("Filter on port range");

    filter_port_begin =  new QSpinBox();
    filter_port_begin->setMinimum(1);
    filter_port_begin->setMaximum(65565);
    filter_port_begin->setValue(1);

    filter_port_end =  new QSpinBox();
    filter_port_end->setMinimum(1);
    filter_port_end->setMaximum(65565);
    filter_port_end->setValue(1024);

    compute_graph_button = new QPushButton("Graph it");




    this->selected_graph = -1;
    this->filter_port_1 = 1;
    this->filter_port_2 = 1024;





    connect(compute_graph_button, SIGNAL(clicked()), this, SLOT(compute_graph()));
    connect(icmp_sip_dip, SIGNAL(toggled(bool)), this, SLOT(icmp_selected(bool)));



    QVBoxLayout *layout = new QVBoxLayout();


    layout->setSpacing(5);
    layout->addWidget(tcp_sip_dip);
    layout->addWidget(udp_sip_dip);
    layout->addWidget(icmp_sip_dip);


    layout->setSpacing(5);

    layout->addWidget(filter_port_label);
    layout->addWidget(filter_port_begin);
    layout->addWidget(filter_port_end);

    layout->addItem(new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding ));

    layout->addWidget(compute_graph_button);

    setLayout(layout);
}



void GraphFeatureSelector::compute_graph()
{
    int feat_active = -1;

    if (tcp_sip_dip->isChecked())
        feat_active = 0;

    else if (udp_sip_dip->isChecked())
        feat_active = 1;

    else if (icmp_sip_dip->isChecked())
        feat_active = 2;


    int port_init = filter_port_begin->value();
    int port_end = filter_port_end->value();


    if  (feat_active == -1)
    {
        QMessageBox::critical(this, QString("No Graph"), QString("No feature selected"), QMessageBox::Ok);
        return;
    }


    emit this->draw_graph_signal(feat_active, port_init, port_end);



}

