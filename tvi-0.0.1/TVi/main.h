#ifndef MAIN_H
#define MAIN_H




#include <QWidget>
#include "timestamp_selected.h"


class MyWidget : public QWidget
{
    Q_OBJECT

public:
    MyWidget(QWidget *parent = 0);


public slots:
    void tab_selection(int pos);

signals:
    void histogram_is_active(bool);
    void graphview_is_active(bool);
    void matrix_is_active(bool);

private:
    timestamp_selected * ts;

};


#endif // MAIN_H
