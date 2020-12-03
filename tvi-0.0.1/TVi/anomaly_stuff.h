#ifndef ANOMALY_STUFF_H
#define ANOMALY_STUFF_H

#include <QWidget>
#include <QObject>

class QProgressBar;
class QListWidget;


class AnomalyList
{

    public:
    AnomalyList(){;}
    ~AnomalyList()
    {
        init_anomaly_timestamp.clear();
        end_anomaly_timestamp.clear();
        intensity.clear();
    }

    std::vector<int> init_anomaly_timestamp;
    std::vector<int> end_anomaly_timestamp;
    std::vector<double> intensity;
};




class AnomalyStuff: public QWidget
{
    Q_OBJECT


public slots:
    void anomaly_list_signal(AnomalyList * anomalies);
    void eigenvalues_signal(float);
    void index_changed_anomaly_list(int);
    void is_anomaly_shown(int anomaly_is_active);

signals:
    void anomaly_selected(int begin, int end);


public:
    AnomalyStuff(QWidget *parent = 0);
    ~AnomalyStuff();

    QProgressBar * energy;
    QListWidget * anomaly_list;


private:
    AnomalyList * anomalylist;

};




#endif // ANOMALY_STUFF_H
