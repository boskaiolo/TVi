#ifndef _FEATURESBOX_
#define _FEATURESBOX_

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>


class QCheckBox;

class FeaturesBox: public QWidget
{
    Q_OBJECT

public:
    FeaturesBox(QWidget *parent = 0);

signals:
    void valueChanged_tcp_sport(int newVal);
    void valueChanged_tcp_dport(int newVal);
    void valueChanged_tcp_sip(int newVal);
    void valueChanged_tcp_dip(int newVal);

    void valueChanged_udp_sport(int newVal);
    void valueChanged_udp_dport(int newVal);
    void valueChanged_udp_sip(int newVal);
    void valueChanged_udp_dip(int newVal);

    void valueChanged_icmp_sip(int newVal);
    void valueChanged_icmp_dip(int newVal);

    void valueChanged_anomaly(int newVal);
    void valueChanged_n_basis(int newVal);

    void valueChanged_log_hist(int newVal);


    void selected_protocol(bool tcp, bool udp, bool icmp);


 private slots:
    void anomaly_selector (bool is_selected);


    void slot_tcp_sport(int newVal);
    void slot_tcp_dport(int newVal);
    void slot_tcp_sip(int newVal);
    void slot_tcp_dip(int newVal);

    void slot_udp_sport(int newVal);
    void slot_udp_dport(int newVal);
    void slot_udp_sip(int newVal);
    void slot_udp_dip(int newVal);

    void slot_icmp_sip(int newVal);
    void slot_icmp_dip(int newVal);

    void clean_all_pushed();
    void select_all_pushed();






private:

    QCheckBox * tcp_sport;
    QCheckBox * tcp_dport;
    QCheckBox * tcp_sip;
    QCheckBox * tcp_dip;

    QCheckBox * udp_sport;
    QCheckBox * udp_dport;
    QCheckBox * udp_sip;
    QCheckBox * udp_dip;

    QCheckBox * icmp_sip;
    QCheckBox * icmp_dip;

    QCheckBox * anomaly;
    QSpinBox * n_basis;
    QCheckBox * logscale;
    QLabel * anomaly_basis_label;

    QPushButton * clean_all;
    QPushButton * select_all;



};

#endif
