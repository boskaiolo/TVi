#ifndef MATRIX_FEATURES_H
#define MATRIX_FEATURES_H


#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>


class MatrixFeatures: public QWidget
{
    Q_OBJECT

public:
    MatrixFeatures(QWidget *parent = 0);

private slots:
    void compute_matrix();

public slots:
    void selected_protocol(bool tcp, bool udp, bool icmp);
    void icmp_selected(bool is_selected);

signals:
    void draw_matrix_signal(int proto, int feat_x, int feat_y, int feat_height, int port_filter_begin, int port_filter_end);

private:

    QRadioButton * check_tcp;
    QRadioButton * check_udp;
    QRadioButton * check_icmp;

    QCheckBox * check_sip;
    QCheckBox * check_dip;
    QCheckBox * check_sport;
    QCheckBox * check_dport;


    QCheckBox * check_flows;
    QCheckBox * check_packets;
    QCheckBox * check_bytes;


    QSpinBox * filter_port_begin;
    QSpinBox * filter_port_end;

    QPushButton * compute_matrix_button;


};

#endif // MATRIX_FEATURES_H
