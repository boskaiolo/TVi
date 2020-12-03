#ifndef HIST_TAB_H
#define HIST_TAB_H

#include <QWidget>

class hist_tab: public QWidget
{
    Q_OBJECT


public:
    hist_tab(QWidget *parent = 0);

signals:
    void loghist_selected (bool isloghist);



};

#endif // HIST_TAB_H
