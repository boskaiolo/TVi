#include <QVBoxLayout>
//#include <QComboBox>
#include <QLabel>
#include <QString>
#include <QCheckBox>
#include <QPalette>
#include <QColor>
#include <QButtonGroup>
#include <QtGui>
#include <QGroupBox>
#include <QSpinBox>
#include <iostream>


#include "featuresbox.h"
#include "colors.h"



QCheckBox * create_qcheckbox (QString name, float r, float g, float b) {
    QPalette p;
    QColor c;

    c.setRgbF(r,g,b);

    QCheckBox * cb = new QCheckBox (name);
    p.setColor(QPalette::WindowText, c);
    cb->setPalette(p);
    cb->setFont(QFont("Times", 12, QFont::Bold));

    return cb;
}



FeaturesBox::FeaturesBox(QWidget * parent)
    : QWidget(parent)
{



    this->setStyleSheet ("QCheckBox { background-color: black }");



    QLabel * title = new QLabel("Select the features to be shown:");
    title->setFont(QFont("Times", 11, QFont::Bold));


    QLabel * anomaly_title = new QLabel("PCA and SNORT anomalies:");
    anomaly_title->setFont(QFont("Times", 11, QFont::Bold));




    tcp_sip = create_qcheckbox("TCP - SIP", COLOR_H_tcp_sip);
    tcp_sport = create_qcheckbox("TCP - SPORT", COLOR_H_tcp_sport);
    tcp_dip = create_qcheckbox("TCP - DIP", COLOR_H_tcp_dip);
    tcp_dport = create_qcheckbox("TCP - DPORT", COLOR_H_tcp_dport);


    udp_sip = create_qcheckbox("UDP - SIP", COLOR_H_udp_sip);
    udp_sport = create_qcheckbox("UDP - SPORT", COLOR_H_udp_sport);
    udp_dip = create_qcheckbox("UDP - DIP", COLOR_H_udp_dip);
    udp_dport = create_qcheckbox("UDP - DPORT", COLOR_H_udp_dport);


    icmp_sip = create_qcheckbox("ICMP - SIP", COLOR_H_icmp_sip);
    icmp_dip = create_qcheckbox("ICMP - DIP", COLOR_H_icmp_dip);

    anomaly = create_qcheckbox("ANOMALY S.", COLOR_H_anomaly);

    n_basis =  new QSpinBox();
    n_basis->setFont(QFont("Times", 11, QFont::Bold));
    n_basis->setValue(3);
    n_basis->setMinimum(1);
    n_basis->setMaximum(10);

    //logscale = create_qcheckbox("LogLog hist", 1.0f, 1.0f, 1.0f);

    anomaly_basis_label = new QLabel("num. of PCA basis");
    anomaly_basis_label->setFont(QFont("Times", 11, QFont::Bold));


    n_basis->setDisabled(true);
    anomaly_basis_label->setDisabled(true);


    clean_all = new QPushButton("Clean all");
    clean_all->setFont(QFont("Times", 10, QFont::Bold));

    select_all = new QPushButton("Select all");
    select_all->setFont(QFont("Times", 10, QFont::Bold));



    connect(tcp_sport, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_tcp_sport(int)));
    connect(tcp_dport, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_tcp_dport(int)));
    connect(tcp_sip, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_tcp_sip(int)));
    connect(tcp_dip, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_tcp_dip(int)));

    connect(udp_sport, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_udp_sport(int)));
    connect(udp_dport, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_udp_dport(int)));
    connect(udp_sip, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_udp_sip(int)));
    connect(udp_dip, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_udp_dip(int)));

    connect(icmp_sip, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_icmp_sip(int)));
    connect(icmp_dip, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_icmp_dip(int)));

    connect(anomaly, SIGNAL(stateChanged(int)), this, SIGNAL(valueChanged_anomaly(int)));
    connect(n_basis, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged_n_basis(int)));

    connect(anomaly, SIGNAL(clicked(bool)), this, SLOT(anomaly_selector(bool)));


    connect(clean_all, SIGNAL(clicked()), this, SLOT(clean_all_pushed()));
    connect(select_all, SIGNAL(clicked()), this, SLOT(select_all_pushed()));


    connect(tcp_sport, SIGNAL(stateChanged(int)), this, SLOT(slot_tcp_sport(int)));
    connect(tcp_dport, SIGNAL(stateChanged(int)), this, SLOT(slot_tcp_dport(int)));
    connect(tcp_sip, SIGNAL(stateChanged(int)), this, SLOT(slot_tcp_sip(int)));
    connect(tcp_dip, SIGNAL(stateChanged(int)), this, SLOT(slot_tcp_dip(int)));

    connect(udp_sport, SIGNAL(stateChanged(int)), this, SLOT(slot_udp_sport(int)));
    connect(udp_dport, SIGNAL(stateChanged(int)), this, SLOT(slot_udp_dport(int)));
    connect(udp_sip, SIGNAL(stateChanged(int)), this, SLOT(slot_udp_sip(int)));
    connect(udp_dip, SIGNAL(stateChanged(int)), this, SLOT(slot_udp_dip(int)));

    connect(icmp_sip, SIGNAL(stateChanged(int)), this, SLOT(slot_icmp_sip(int)));
    connect(icmp_dip, SIGNAL(stateChanged(int)), this, SLOT(slot_icmp_dip(int)));






    QHBoxLayout * anomaly_basis_layout = new QHBoxLayout();
    anomaly_basis_layout->addWidget(anomaly_basis_label);
    anomaly_basis_layout->addStretch(1);
    anomaly_basis_layout->addWidget(n_basis);


    QHBoxLayout *buttons_layout = new QHBoxLayout();
    buttons_layout->addWidget(clean_all);
    anomaly_basis_layout->addStretch(1);
    buttons_layout->addWidget(select_all);


    QVBoxLayout *layout = new QVBoxLayout();

    layout->addWidget(title);

    layout->addWidget(tcp_sip);
    layout->addWidget(tcp_sport);
    layout->addWidget(tcp_dip);
    layout->addWidget(tcp_dport);
    layout->addWidget(udp_sip);
    layout->addWidget(udp_sport);
    layout->addWidget(udp_dip);
    layout->addWidget(udp_dport);

    layout->addWidget(icmp_sip);
    layout->addWidget(icmp_dip);
    layout->addSpacing(1);

    layout->addWidget(anomaly_title);

    layout->addWidget(anomaly);
    layout->addLayout(anomaly_basis_layout);


    //layout->addSpacing(1);
    layout->addLayout(buttons_layout);

    setLayout(layout);
}



void FeaturesBox::anomaly_selector (bool is_selected)
{
    if (is_selected == false)
    {
        n_basis->setDisabled(true);
        anomaly_basis_label->setDisabled(true);
    }
    else
    {
        n_basis->setDisabled(false);
        anomaly_basis_label->setDisabled(false);
    }

}



void FeaturesBox::slot_tcp_sport(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(true, false, false);
}
void FeaturesBox::slot_tcp_dport(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(true, false, false);
}
void FeaturesBox::slot_tcp_sip(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(true, false, false);
}
void FeaturesBox::slot_tcp_dip(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(true, false, false);
}



void FeaturesBox::slot_udp_sport(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(false, true, false);
}
void FeaturesBox::slot_udp_dport(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(false, true, false);
}
void FeaturesBox::slot_udp_sip(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(false, true, false);
}
void FeaturesBox::slot_udp_dip(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(false, true, false);
}



void FeaturesBox::slot_icmp_sip(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(false, false, true);
}
void FeaturesBox::slot_icmp_dip(int newVal)
{
    if (newVal!=0)
        emit selected_protocol(false, false, true);
}



void FeaturesBox::clean_all_pushed()
{
    this->tcp_sip->setChecked(false);
    this->tcp_sport->setChecked(false);
    this->tcp_dip->setChecked(false);
    this->tcp_dport->setChecked(false);


    this->udp_sip->setChecked(false);
    this->udp_sport->setChecked(false);
    this->udp_dip->setChecked(false);
    this->udp_dport->setChecked(false);


    this->icmp_sip->setChecked(false);
    this->icmp_dip->setChecked(false);

    this->anomaly->setChecked(false);
    this->n_basis->setDisabled(true);


    emit(valueChanged_tcp_sip(0));
    emit(valueChanged_tcp_sport(0));
    emit(valueChanged_tcp_dip(0));
    emit(valueChanged_tcp_dport(0));


    emit(valueChanged_udp_sip(0));
    emit(valueChanged_udp_sport(0));
    emit(valueChanged_udp_dip(0));
    emit(valueChanged_udp_dport(0));


    emit(valueChanged_icmp_sip(0));
    emit(valueChanged_icmp_dip(0));

    emit(valueChanged_anomaly(0));
}

void FeaturesBox::select_all_pushed()
{
    this->tcp_sip->setChecked(true);
    this->tcp_sport->setChecked(true);
    this->tcp_dip->setChecked(true);
    this->tcp_dport->setChecked(true);


    this->udp_sip->setChecked(true);
    this->udp_sport->setChecked(true);
    this->udp_dip->setChecked(true);
    this->udp_dport->setChecked(true);


    this->icmp_sip->setChecked(true);
    this->icmp_dip->setChecked(true);

    this->anomaly->setChecked(false);
    this->n_basis->setDisabled(true);


    emit(valueChanged_tcp_sip(1));
    emit(valueChanged_tcp_sport(1));
    emit(valueChanged_tcp_dip(1));
    emit(valueChanged_tcp_dport(1));


    emit(valueChanged_udp_sip(1));
    emit(valueChanged_udp_sport(1));
    emit(valueChanged_udp_dip(1));
    emit(valueChanged_udp_dport(1));


    emit(valueChanged_icmp_sip(1));
    emit(valueChanged_icmp_dip(1));

    emit(valueChanged_anomaly(0));
}
