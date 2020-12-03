#ifndef ENTROPY_VIEWER_H
#define ENTROPY_VIEWER_H

#include <QGLWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMouseEvent>
#include <QMenu>
#include <vector>
#include "anomaly_stuff.h"
#include "timestamp_selected.h"

 class EntropyViewer : public QGLWidget
 {
     Q_OBJECT

 public:
     EntropyViewer(QWidget *parent = 0, timestamp_selected * ts = NULL);
     ~EntropyViewer();


 protected:
     void initializeGL();
     void paintGL();
     void resizeGL(int width, int height);
     void mousePressEvent(QMouseEvent *event);
     void mouseReleaseEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);



 public slots:
     void visualization_tcp_sport (int newState);
     void visualization_tcp_dport (int newState);
     void visualization_tcp_sip (int newState);
     void visualization_tcp_dip (int newState);

     void visualization_udp_sport (int newState);
     void visualization_udp_dport (int newState);
     void visualization_udp_sip (int newState);
     void visualization_udp_dip (int newState);

     void visualization_icmp_sip (int newState);
     void visualization_icmp_dip (int newState);

     void visualization_anomaly (int newState);
     void visualization_n_basis (int newState);

     void anomaly_selected(int, int);

     void selected_histograms(std::vector<int> timestamps);

     void time_forward();
     void time_backward();


     void open_wireshark();
     void open_etherape();

     void anomaly_menu_triggered ( QAction * action );



signals:
     void time_click(int, int, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);
     void anomaly_list_signal(AnomalyList * );
     void energy_eigen_signal(float);
     void send_log(QString);


 private:

     timestamp_selected * ts;

     int height;
     int width;
     QPoint pressPos;
     QPoint releasePos;
     QPoint actualPos;
     QMenu * menu;
     bool is_mouse_button_pressed;

     std::vector<int> timestamps_V;

     std::vector<float> tcp_sport_V;
     std::vector<float> tcp_dport_V;
     std::vector<float> tcp_sip_V;
     std::vector<float> tcp_dip_V;

     std::vector<float> udp_sport_V;
     std::vector<float> udp_dport_V;
     std::vector<float> udp_sip_V;
     std::vector<float> udp_dip_V;

     std::vector<float> icmp_sip_V;
     std::vector<float> icmp_dip_V;

     std::vector<std::vector<float> > anomalies_V;
     int n_basis;
     std::vector<double> eigenvalues_V;



     std::vector<std::string> anomaly_snort_name;
     std::vector<QAction *> anomaly_snort_action;
     std::vector<bool> anomaly_snort_ischeck;
     std::vector<std::vector<float> > snort_anomalies_matrix;

     bool show_tcp_sport;
     bool show_tcp_dport;
     bool show_tcp_sip;
     bool show_tcp_dip;

     bool show_udp_sport;
     bool show_udp_dport;
     bool show_udp_sip;
     bool show_udp_dip;

     bool show_icmp_sip;
     bool show_icmp_dip;

     bool show_anomaly;


     int init_timestamp_click;
     int end_timestamp_click;


     std::vector<int> selected_histograms_V;



     void compute_timestamps_and_event();
     void compute_energy_eigenvalues();
     void fetch_data (QString host, QString name, QString user, QString pass);
     float cerca_max_value();
     float compute_anomaly_max();
     void compute_anomaly_list();



 };

#endif // ENTROPY_VIEWER_H
