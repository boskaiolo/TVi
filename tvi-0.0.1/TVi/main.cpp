#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QWidget>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QDebug>
#include <QDesktopWidget>

#include "main.h"
#include "featuresbox.h"
#include "entropy_viewer.h"
#include "histogram_viewer.h"
#include "anomaly_stuff.h"
#include "graphfeatureselector.h"
#include "graphvisualizer.h"
#include "matrix_features.h"
#include "matrix_visualizer.h"
#include "timeforwardbackward.h"
#include "tools.h"
#include "hist_tab.h"


void MyWidget::tab_selection(int pos)
{
    if (pos == 0)
    {
        emit this->histogram_is_active(true);
        emit this->graphview_is_active(false);
        emit this->matrix_is_active(false);
    }
    else if (pos == 1)
    {
        emit this->histogram_is_active(false);
        emit this->graphview_is_active(true);
        emit this->matrix_is_active(false);
    }
    else if (pos ==2)
    {
        emit this->histogram_is_active(false);
        emit this->graphview_is_active(false);
        emit this->matrix_is_active(true);
    }
}



MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
{

    ts = new timestamp_selected();

    FeaturesBox * features = new FeaturesBox(this);

    hist_tab * hist_selector = new hist_tab(this);
    GraphFeatureSelector * graph_features = new GraphFeatureSelector(this);
    MatrixFeatures * matrix_features = new MatrixFeatures(this);

    EntropyViewer * entropyviewer = new EntropyViewer(this, ts);
    HistogramViewer * histview = new HistogramViewer(this);
    AnomalyStuff * anomalystuff = new AnomalyStuff(this);
    anomalystuff->setHidden(true);

    graphVisualizer * graphViewer = new graphVisualizer(this);
    graphViewer->setHidden(true);


    MatrixVisualizer * matrix_visualizer = new MatrixVisualizer(this);
    matrix_visualizer->setHidden(true);



    TimeForwardBackward * timebuttons = new TimeForwardBackward(this);






    QTabWidget * hist_tab_selector = new QTabWidget();


    hist_tab_selector->setToolTip("Select the visualization mode and its parameters");

    int index;

    index = hist_tab_selector->addTab(hist_selector, "HIST");
    hist_tab_selector->setTabToolTip(index, "Histogram visualizer mode");

    index = hist_tab_selector->addTab(graph_features, "GRAPH");
    hist_tab_selector->setTabToolTip(index, "Graph visualizer mode");

    index = hist_tab_selector->addTab(matrix_features, "MATRIX");
    hist_tab_selector->setTabToolTip(index, "Matrix visualizer mode");


    QPlainTextEdit * log = new QPlainTextEdit("messages");
    log->setMaximumBlockCount (1);
    log->setReadOnly(true);
    log->setMaximumHeight(30);



    connect(hist_tab_selector, SIGNAL(currentChanged(int)), this, SLOT(tab_selection(int)));
    connect(this, SIGNAL(histogram_is_active(bool)), histview, SLOT(histogram_is_active(bool)));
    connect(this, SIGNAL(graphview_is_active(bool)), graphViewer, SLOT(graph_is_active(bool)));
    connect(this, SIGNAL(matrix_is_active(bool)), matrix_visualizer, SLOT(matrix_is_active(bool)));

    connect(features, SIGNAL(valueChanged_tcp_sport(int)), entropyviewer, SLOT(visualization_tcp_sport(int)));
    connect(features, SIGNAL(valueChanged_tcp_dport(int)), entropyviewer, SLOT(visualization_tcp_dport(int)));
    connect(features, SIGNAL(valueChanged_tcp_sip(int)), entropyviewer, SLOT(visualization_tcp_sip(int)));
    connect(features, SIGNAL(valueChanged_tcp_dip(int)), entropyviewer, SLOT(visualization_tcp_dip(int)));

    connect(features, SIGNAL(valueChanged_udp_sport(int)), entropyviewer, SLOT(visualization_udp_sport(int)));
    connect(features, SIGNAL(valueChanged_udp_dport(int)), entropyviewer, SLOT(visualization_udp_dport(int)));
    connect(features, SIGNAL(valueChanged_udp_sip(int)), entropyviewer, SLOT(visualization_udp_sip(int)));
    connect(features, SIGNAL(valueChanged_udp_dip(int)), entropyviewer, SLOT(visualization_udp_dip(int)));

    connect(features, SIGNAL(valueChanged_icmp_sip(int)), entropyviewer, SLOT(visualization_icmp_sip(int)));
    connect(features, SIGNAL(valueChanged_icmp_dip(int)), entropyviewer, SLOT(visualization_icmp_dip(int)));

    connect(features, SIGNAL(valueChanged_anomaly(int)), entropyviewer, SLOT(visualization_anomaly(int)));
    connect(features, SIGNAL(valueChanged_n_basis(int)), entropyviewer, SLOT(visualization_n_basis(int)));


    connect(features, SIGNAL(selected_protocol(bool,bool,bool)), matrix_features, SLOT(selected_protocol(bool,bool,bool)));
    connect(features, SIGNAL(selected_protocol(bool,bool,bool)), graph_features, SLOT(selected_protocol(bool,bool,bool)));

    connect(entropyviewer, SIGNAL(time_click(int,int,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool)), histview, SLOT(time_clicked(int, int, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool)));



    connect(hist_selector, SIGNAL(loghist_selected(bool)), histview, SLOT(log_hist_clicked(bool)));
    connect(features, SIGNAL(valueChanged_anomaly(int)), anomalystuff, SLOT(is_anomaly_shown(int)));


    connect(entropyviewer, SIGNAL(anomaly_list_signal(AnomalyList*)), anomalystuff, SLOT(anomaly_list_signal(AnomalyList*)));
    connect(entropyviewer, SIGNAL(energy_eigen_signal(float)), anomalystuff, SLOT(eigenvalues_signal(float)));
    connect(histview, SIGNAL(send_log(QString)), log, SLOT(appendPlainText(QString)));
    connect(entropyviewer, SIGNAL(send_log(QString)), log, SLOT(appendPlainText(QString)));
    connect(entropyviewer, SIGNAL(send_log(QString)), log, SLOT(appendPlainText(QString)));
    connect(anomalystuff, SIGNAL(anomaly_selected(int,int)), entropyviewer, SLOT(anomaly_selected(int,int)));
    connect(histview, SIGNAL(selected_timestamps(std::vector<int>)), entropyviewer, SLOT(selected_histograms(std::vector<int>)));

    connect(graph_features, SIGNAL(draw_graph_signal(int,int,int)), graphViewer, SLOT(draw_graph_feature(int,int,int)));
    connect(entropyviewer, SIGNAL(time_click(int,int,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool)), graphViewer, SLOT(time_clicked(int,int)));
    connect(matrix_features, SIGNAL(draw_matrix_signal(int,int,int,int,int,int)), matrix_visualizer, SLOT(draw_matrix_on_signal(int,int,int,int,int,int)));
    connect(entropyviewer, SIGNAL(time_click(int,int,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool)), matrix_visualizer, SLOT(time_clicked(int,int)));
    connect(timebuttons, SIGNAL(moveForward()), entropyviewer, SLOT(time_forward()));
    connect(timebuttons, SIGNAL(moveBackward()), entropyviewer, SLOT(time_backward()));

    connect(timebuttons, SIGNAL(open_etherape()), entropyviewer, SLOT(open_etherape()));
    connect(timebuttons, SIGNAL(open_wireshark()), entropyviewer, SLOT(open_wireshark()));



    entropyviewer->setMaximumHeight(300);
    anomalystuff->setMaximumHeight(310);


    QHBoxLayout * features_and_buttons = new QHBoxLayout();
    features_and_buttons->addWidget(entropyviewer);
    features_and_buttons->addWidget(timebuttons);
    features_and_buttons->setStretch(0,12);




    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(features, 0, 0);
    gridLayout->addWidget(hist_tab_selector, 1, 0);
    gridLayout->addLayout(features_and_buttons, 0, 1);


    gridLayout->addWidget(graphViewer,1, 1, 2, 1);
    gridLayout->addWidget(histview,1, 1, 2, 1);
    gridLayout->addWidget(matrix_visualizer,1, 1, 2, 1);


    gridLayout->addWidget(anomalystuff,2, 0);


    //gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 10);
    gridLayout->setRowStretch(2,10);

    gridLayout->addWidget(log, 3, 0, 1, 2);
    setLayout(gridLayout);



    //this->setWindowIcon(QIcon(":/images/icon.icns"));




}




int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyWidget widget;
    widget.setWindowTitle("TVi: A Visual Querying System for Network Monitoring and Anomaly Detection");

    QRect screen = QApplication::desktop()->availableGeometry();

    widget.setGeometry(screen.x(), screen.y(), screen.width(), screen.height()-20);
    widget.show();
    return app.exec();
}
