#include "matrix_features.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include  <QDebug>

#include <iostream>
#include <vector>



QCheckBox * create_qcheckbox_wocolor (QString name)
{
    QCheckBox * cb = new QCheckBox (name);
    cb->setFont(QFont("Times", 14, QFont::Bold));
    cb->setTristate(false);
    return cb;
}

QRadioButton * create_qradiobutton_wocolor (QString name)
{
    QRadioButton * rb = new QRadioButton (name);
    rb->setFont(QFont("Times", 14, QFont::Bold));
    return rb;
}



void MatrixFeatures::selected_protocol(bool tcp, bool udp, bool icmp)
{
    if (tcp == true)
        check_tcp->setChecked(true);
    else if (udp == true)
       check_udp->setChecked(true);
    else if (icmp == true)
       check_icmp->setChecked(true);
}

MatrixFeatures::MatrixFeatures(QWidget * parent)
    : QWidget(parent)
{


    QLabel * protocol_label = new QLabel("Select protocol");
    check_tcp = create_qradiobutton_wocolor("TCP");
    check_udp = create_qradiobutton_wocolor("UDP");
    check_icmp = create_qradiobutton_wocolor("ICMP");


    QLabel * features_label = new QLabel("Select 2 features");
    check_sip = create_qcheckbox_wocolor("SIP");
    check_dip = create_qcheckbox_wocolor("DIP");
    check_sport = create_qcheckbox_wocolor("SPORT");
    check_dport = create_qcheckbox_wocolor("DPORT");



    QLabel * height_label = new QLabel("(opz) select one height feat");
    check_flows = create_qcheckbox_wocolor("# flows");
    check_packets = create_qcheckbox_wocolor("# packets");
    check_bytes = create_qcheckbox_wocolor("# bytes");



    QLabel * filter_port_label = new QLabel("Filter ports in");

    filter_port_begin =  new QSpinBox();
    filter_port_begin->setMinimum(1);
    filter_port_begin->setMaximum(65565);
    filter_port_begin->setValue(1);

    filter_port_end =  new QSpinBox();
    filter_port_end->setMinimum(1);
    filter_port_end->setMaximum(65565);
    filter_port_end->setValue(1024);



    compute_matrix_button = new QPushButton("Show Matrix");




    connect(compute_matrix_button, SIGNAL(clicked()), this, SLOT(compute_matrix()));


    connect(check_icmp, SIGNAL(toggled(bool)), this, SLOT(icmp_selected(bool)));


    QVBoxLayout * port_select_layout = new QVBoxLayout();
    QHBoxLayout * port_selector = new QHBoxLayout();


    port_select_layout->addWidget(filter_port_begin);
    port_select_layout->addWidget(filter_port_end);

    port_selector->addWidget(filter_port_label);
    port_selector->addLayout(port_select_layout);


    QVBoxLayout *layout = new QVBoxLayout();

    layout->setSpacing(5);
    layout->addWidget(protocol_label);
    layout->addWidget(check_tcp);
    layout->addWidget(check_udp);
    layout->addWidget(check_icmp);

    layout->setSpacing(5);
    layout->addWidget(features_label);
    layout->addWidget(check_sip);
    layout->addWidget(check_dip);
    layout->addWidget(check_sport);
    layout->addWidget(check_dport);

    layout->setSpacing(5);
    layout->addWidget(height_label);
    layout->addWidget(check_flows);
    layout->addWidget(check_packets);
    layout->addWidget(check_bytes);


    layout->addLayout(port_selector);

    layout->addWidget(compute_matrix_button);

    setLayout(layout);
}


void MatrixFeatures::icmp_selected(bool is_selected)
{
    if (is_selected == true)
    {
        check_sip->setChecked(true);
        check_dip->setChecked(true);
        check_sport->setDisabled(true);
        check_dport->setDisabled(true);
        filter_port_begin->setDisabled(true);
        filter_port_end->setDisabled(true);
    }
    else
    {
            check_sport->setEnabled(true);
            check_dport->setEnabled(true);
            filter_port_begin->setEnabled(true);
            filter_port_end->setEnabled(true);
     }
}



void MatrixFeatures::compute_matrix()
{

    int proto = -1;

    if (check_tcp->isChecked())
        proto = 6;
    else if (check_udp->isChecked())
        proto = 17;
    else if (check_icmp->isChecked())
        proto = 1;


    if  (proto == -1)
    {
        QMessageBox::critical(this, QString("No Matrix"), QString("No protocol selected"), QMessageBox::Ok);
        return;
    }

    std::vector<int> feat_active;
    feat_active.clear();

    if (check_sip->isChecked())
        feat_active.push_back(0);
    if (check_sport->isChecked())
        feat_active.push_back(1);
    if (check_dip->isChecked())
        feat_active.push_back(2);
    if (check_dport->isChecked())
        feat_active.push_back(3);


    if  (feat_active.size() != 2)
    {
        QMessageBox::critical(this, QString("No Matrix"), QString("Please, select 2 features for the matrix"), QMessageBox::Ok);
        return;
    }

    int feat_height = -1;


    if (check_flows->isChecked())
        feat_height = 0;
    else if (check_packets->isChecked())
        feat_height = 1;
    else if (check_bytes->isChecked())
        feat_height = 2;


    int port_filter_begin = this->filter_port_begin->value();
    int port_filter_end = this->filter_port_end->value();

    emit draw_matrix_signal(proto, feat_active[0], feat_active[1], feat_height, port_filter_begin, port_filter_end);
    feat_active.clear();

}

