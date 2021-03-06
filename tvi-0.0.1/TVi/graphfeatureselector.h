#ifndef GRAPHFEATURESELECTOR_H
#define GRAPHFEATURESELECTOR_H

#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QPushButton>


class GraphFeatureSelector: public QWidget
{
    Q_OBJECT

public:
    GraphFeatureSelector(QWidget *parent = 0);



private slots:

    void compute_graph();
    void icmp_selected(bool is_selected);


public slots:
    void selected_protocol(bool tcp, bool udp, bool icmp);




signals:
    void draw_graph_signal(int, int, int);


private:


    int selected_graph;
    int filter_port_1;
    int filter_port_2;


    QRadioButton * tcp_sip_dip;
    QRadioButton * udp_sip_dip;
    QRadioButton * icmp_sip_dip;

    QSpinBox * filter_port_begin;
    QSpinBox * filter_port_end;

    QPushButton * compute_graph_button;



};


#endif // GRAPHFEATURESELECTOR_H
