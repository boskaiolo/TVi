#include "anomaly_stuff.h"
#include "tools.h"


#include <QProgressBar>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

AnomalyStuff::AnomalyStuff(QWidget * parent)
    : QWidget(parent)
{

    energy = new QProgressBar(this);
    energy->setTextVisible(true);
    energy->setMinimum(0);
    energy->setMaximum(100);
    energy->setFormat("Eig energy: %p (%v)");


    energy->setValue(1);

    anomaly_list = new QListWidget(this);
    anomaly_list->setMinimumHeight(300);
    anomaly_list->setSpacing(8);


    QLabel * energy_pca_label = new QLabel("Eigenvalues' energy");
    QLabel * anomaly_list_label = new QLabel("Anomaly list: ");



    QVBoxLayout *layout = new QVBoxLayout;


    layout->addWidget(energy_pca_label);
    layout->addWidget(energy);
    layout->addSpacing(2);
    layout->addWidget(anomaly_list_label);
    layout->addWidget(anomaly_list);



    setLayout(layout);


    anomaly_list->setMaximumWidth(180);
    anomaly_list->setMinimumWidth(180);



    anomalylist = NULL;

    connect(anomaly_list, SIGNAL(currentRowChanged(int)), this, SLOT(index_changed_anomaly_list(int)));

}


AnomalyStuff::~AnomalyStuff()
{
    if (anomalylist != NULL)
        delete anomalylist;
}



void AnomalyStuff::is_anomaly_shown(int anomaly_is_active)
{
    this->setHidden(anomaly_is_active==0?true:false);
}



void AnomalyStuff::eigenvalues_signal(float p)
{
    energy->setValue(p);
}


void AnomalyStuff::index_changed_anomaly_list(int row)
{

    qWarning() << "selected row" << row;

    if (row<0)
        return;

    if (anomalylist != NULL)
    {
        int t0 = anomalylist->init_anomaly_timestamp[row];
        int t1 =anomalylist->end_anomaly_timestamp[row];
        emit this->anomaly_selected(t0, t1);
    }
}


void AnomalyStuff::anomaly_list_signal(AnomalyList * anomalies)
{


    if (anomalylist != NULL)
        delete anomalylist;
    anomalylist = anomalies;

    anomaly_list->clear();
    QString s;


    for (size_t i = 0; i< anomalies->init_anomaly_timestamp.size(); i++)
    {
        s = "* ANOMALY\n";
        s += ctime(((time_t*) &anomalies->init_anomaly_timestamp[i]));
        s += " -> ";
        s += ctime(((time_t*) &anomalies->end_anomaly_timestamp[i]));
        s += " I =";
        s += double2str(anomalies->intensity[i]);

        QListWidgetItem * actualitem = new QListWidgetItem(s, anomaly_list);
        actualitem->setFont(QFont("Helvetica", 12));
    }
}
