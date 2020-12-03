#include "hist_tab.h"

#include <QCheckBox>
#include <QVBoxLayout>
#include <colors.h>


hist_tab::hist_tab(QWidget * parent)
    : QWidget(parent)
{

    //this->setStyleSheet ("QCheckBox { background-color: black }");


    QPalette p;
    QColor c;

    c.setRgbF(0.0f, 0.0f, 0.0f);

    QCheckBox * loghist = new QCheckBox ("LogLog hist");
    p.setColor(QPalette::WindowText, c);
    loghist->setPalette(p);
    loghist->setFont(QFont("Times", 14, QFont::Bold));

    connect(loghist, SIGNAL(clicked(bool)), this, SIGNAL(loghist_selected(bool)));


    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(loghist);
    layout->setAlignment(loghist,Qt::AlignTop);

    setLayout(layout);
}

