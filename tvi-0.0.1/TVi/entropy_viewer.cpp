
#include <QtGui>
#include <QtOpenGL>
#include <IOstream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMenu>
#include <GLUT/glut.h>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <vector>
#include <float.h>



#include "entropy_viewer.h"
#include "db_credential.h"
#include "colors.h"
#include "tools.h"
#include "anomaly_stuff.h"




EntropyViewer::EntropyViewer(QWidget *parent, timestamp_selected * ts)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    this->fetch_data (DB_host, DB_name, DB_user, DB_pass);
    show_tcp_sport = false;
    show_tcp_dport = false;
    show_tcp_sip = false;
    show_tcp_dip = false;

    show_udp_sport = false;
    show_udp_dport = false;
    show_udp_sip = false;
    show_udp_dip = false;

    show_icmp_sip = false;
    show_icmp_dip = false;

    show_anomaly = false;


    this->ts = ts;



    n_basis = 3;


    this->setMouseTracking(true);
    this->is_mouse_button_pressed = false;


    this->setFocusPolicy( Qt::StrongFocus);
    this->setEnabled(true);


    menu = new QMenu(QString("Snort selection"));
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(anomaly_menu_triggered(QAction*)));

}




EntropyViewer::~EntropyViewer()
{

    timestamps_V.clear();

    tcp_sport_V.clear();
    tcp_dport_V.clear();
    tcp_sip_V.clear();
    tcp_dip_V.clear();

    udp_sport_V.clear();
    udp_dport_V.clear();
    udp_sip_V.clear();
    udp_dip_V.clear();

    icmp_sip_V.clear();
    icmp_dip_V.clear();
    eigenvalues_V.clear();

    for (size_t i=0; i<anomalies_V.size(); i++){
        anomalies_V[i].clear();
    }
    anomalies_V.clear();

    this->anomaly_snort_ischeck.clear();
    this->anomaly_snort_name.clear();
    this->anomaly_snort_action.clear();
}


void EntropyViewer::initializeGL()
{
    std::cout << "[BEGIN] Init" << std::endl;
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTranslatef(0.0f, 0.0f, -5.0f);

    std::cout << "[END] Init " << std::endl;


    qWarning() << (char*) glGetString(GL_RENDERER);
    qWarning() << (char*) glGetString(GL_VENDOR);
    qWarning() << (char*) glGetString(GL_VERSION);
    qWarning() << (char*) glGetString(GL_EXTENSIONS);

 }


void EntropyViewer::open_wireshark()
{

    int delta_t = timestamps_V[1]-timestamps_V[0];

    QString files;

    char tempfile[] = "temp.pcap";

    for (int i=init_timestamp_click; i<=end_timestamp_click; i+=delta_t)
    {
        files += QString(PCAP_PATH) + QString("/") + int2str(i) + QString(".pcap ");
    }

    remove(tempfile);


    QString command;

    command = QString(MERGECAP_COMMAND) + QString(" -a -w ") + QString(tempfile) + QString(" ") + files;
    qWarning() << command;

    system(command.toStdString().c_str());

    command.clear();
    command = QString(WIRESHARK_COMMAND) + QString(" -r ") + QString(tempfile);
    system("pwd");
    system(command.toStdString().c_str());


    remove(tempfile);

}

void EntropyViewer::open_etherape()
{

    int delta_t = timestamps_V[1]-timestamps_V[0];

    QString files;

    char tempfile[] = "temp.pcap";

    for (int i=init_timestamp_click; i<=end_timestamp_click; i+=delta_t)
    {
        files += QString(PCAP_PATH) + QString("/") + int2str(i) + QString(".pcap ");
    }

    remove(tempfile);


    QString command;

    command = QString(MERGECAP_COMMAND) + QString(" -a -w ") + QString(tempfile) + QString(" ") + files;
    qWarning() << command;

    system(command.toStdString().c_str());

    command.clear();
    command = QString(ETHERAPE_COMMAND) + QString(" -r ") + QString(tempfile);
    system(command.toStdString().c_str());

    remove(tempfile);

}





void EntropyViewer::resizeGL(int width, int height)
 {
    this->width = width;
    this->height = height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0);
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 5000.0);
    glMatrixMode(GL_MODELVIEW);

    std::cout << "W = " << width << "  H = " << height << std::endl;
 }

void EntropyViewer::paintGL()
{

    std::cout << "[BEGIN] PAINT " << std::endl;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(1.0f);


    if ( timestamps_V.size()>0) {
       float max_val = cerca_max_value();
        float scale = 8.0/max_val;

        int len_hist = timestamps_V.size();
        float delta_bin = 10.0/(float) len_hist;




        glColor3f(1.0f, 1.0f, 1.0f);
        renderText (5, 30, QVariant(max_val).toString().toStdString().c_str());
        glLineWidth(1.5f);




        // Axis
        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(-5.0f, -4.0f, 0.0f);
        glVertex3f(5.0f, -4.0f, 0.0f);
        glEnd();

        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(-4.999f, -4.999f, 0.0f);
        glVertex3f(-4.999f, 4.999f, 0.0f);
        glEnd();


        if (show_anomaly == true) {

            std::cout << "Disp Anomaly with K = " << n_basis+1 << std::endl;
            std::cout << "Matrix of anomalies is " << anomalies_V.size() << " x "<< anomalies_V[0].size() << std::endl;

            glColor3f (COLOR_H_anomaly);
            glLineStipple(1, 0x3F07);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINE_STRIP);

            float anomaly_scale = 8.0/compute_anomaly_max();

            for (size_t i = 0; i< anomalies_V[n_basis].size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + anomaly_scale*anomalies_V[n_basis][i], 0.0f);
            }
            glEnd();
            glDisable(GL_LINE_STIPPLE);





            std::vector< float > snort_anomaly(timestamps_V.size(), 0.0f);
            for (size_t i = 0; i<anomaly_snort_name.size(); i++)
            {
                if (this->anomaly_snort_ischeck[i])
                {
                    for (size_t c = 0; c<timestamps_V.size(); c++)
                        snort_anomaly[c] += this->snort_anomalies_matrix[i][c];


                    for (size_t i = 0; i< snort_anomaly.size(); i++)
                            std::cout << snort_anomaly[i] << " ";
                    std::cout << std::endl ;


                }
            }
            float max_snort_val = *max_element(snort_anomaly.begin(),snort_anomaly.end());

            qWarning() << max_snort_val;


            glColor3f (1.0f, 1.0f, 1.0f);
            glLineWidth(1.0f);
            glLineStipple(1, 0x3F07);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINE_STRIP);

            float snort_anomaly_scale = 8.0/max_snort_val;

            for (size_t i = 0; i< anomalies_V[n_basis].size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + snort_anomaly_scale*snort_anomaly[i], 0.0f);
            }
            glEnd();
            glDisable(GL_LINE_STIPPLE);
            glLineWidth(1.5f);




            snort_anomaly.clear();


        }

        if (show_tcp_sport == true) {

            glColor3f (COLOR_H_tcp_sport);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< tcp_sport_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*tcp_sport_V[i], 0.0f);
            }
            glEnd();
        }
        if (show_tcp_dport == true) {

            glColor3f (COLOR_H_tcp_dport);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< tcp_dport_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*tcp_dport_V[i], 0.0f);
            }
            glEnd();
        }
        if (show_tcp_sip == true) {

            glColor3f (COLOR_H_tcp_sip);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< tcp_sip_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*tcp_sip_V[i], 0.0f);
            }
            glEnd();
        }
        if (show_tcp_dip == true) {

            glColor3f (COLOR_H_tcp_dip);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< tcp_dip_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*tcp_dip_V[i], 0.0f);
            }
            glEnd();
        }





        if (show_udp_sport == true) {

            glColor3f (COLOR_H_udp_sport);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< udp_sport_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*udp_sport_V[i], 0.0f);
            }
            glEnd();
        }
        if (show_udp_dport == true) {

            glColor3f (COLOR_H_udp_dport);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< udp_dport_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*udp_dport_V[i], 0.0f);
            }
            glEnd();
        }
        if (show_udp_sip == true) {

            glColor3f (COLOR_H_udp_sip);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< udp_sip_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*udp_sip_V[i], 0.0f);
            }
            glEnd();
        }
        if (show_udp_dip == true) {

            glColor3f (COLOR_H_udp_dip);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< udp_dip_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*udp_dip_V[i], 0.0f);
            }
            glEnd();
        }






        if (show_icmp_sip == true) {

            glColor3f (COLOR_H_icmp_sip);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< icmp_sip_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*icmp_sip_V[i], 0.0f);
            }
            glEnd();
        }
        if (show_icmp_dip == true) {

            glColor3f (COLOR_H_icmp_dip);

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i< icmp_dip_V.size(); i++) {
                glVertex3f(-5.0f + i*delta_bin, -4.0f + scale*icmp_dip_V[i], 0.0f);
            }
            glEnd();
        }





        if (this->is_mouse_button_pressed == true)
        {
            float initial_x = (float) pressPos.x() / (float) this->width;
            float now_x = (float) actualPos.x() / (float) this->width;
            if (now_x == initial_x)
            {
               glColor3f(1.0f, 1.0f, 1.0f);
               glBegin(GL_LINES);
               glVertex3f(-5.0f + 10*initial_x, -4.0f, 0.0f);
               glVertex3f(-5.0f + 10*initial_x, 4.0f, 0.0f);
               glEnd();
            }
            else
            {
               glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
               glBegin(GL_QUADS);
               glVertex3f(-5.0f + 10*initial_x, -4.0f, 0.0f);
               glVertex3f(-5.0f + 10*initial_x, 4.0f, 0.0f);
               glVertex3f(-5.0f + 10*now_x, 4.0f, 0.0f);
               glVertex3f(-5.0f + 10*now_x, -4.0f, 0.0f);
               glColor3f(1.0f,1.0f,1.0f);
               glEnd();
           }
        }
        else
        {
            float initial_x = (float) pressPos.x() / (float) this->width;
            float now_x = (float) releasePos.x() / (float) this->width;
            if (now_x == initial_x)
            {
               glColor3f(1.0f, 1.0f, 1.0f);
               glBegin(GL_LINES);
               glVertex3f(-5.0f + 10*initial_x, -4.0f, 0.0f);
               glVertex3f(-5.0f + 10*initial_x, 4.0f, 0.0f);
               glEnd();
            }
            else
            {
               glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
               glBegin(GL_QUADS);
               glVertex3f(-5.0f + 10*initial_x, -4.0f, 0.0f);
               glVertex3f(-5.0f + 10*initial_x, 4.0f, 0.0f);
               glVertex3f(-5.0f + 10*now_x, 4.0f, 0.0f);
               glVertex3f(-5.0f + 10*now_x, -4.0f, 0.0f);
               glColor3f(1.0f,1.0f,1.0f);
               glEnd();
           }
        }

        float percent_x = (float) actualPos.x() / (float) this->width;
        int current_timestamp = timestamps_V[ round((float)len_hist * percent_x) ];

        glColor3f(1.0f, 1.0f, 1.0f);
        //QString s = QString(ctime((time_t *) &current_timestamp)) + QString(" - ") + QString (int2str(current_timestamp));
        QDateTime dt;
        dt.setTime_t(current_timestamp);
        this->renderText(-1.0f,-4.9,0.0f, (dt.toString() + " - " + QString (int2str(current_timestamp))).toStdString().c_str() );


            for (size_t i=0; i<this->selected_histograms_V.size(); i++)
            {
                float percent_x_ts = (float)( selected_histograms_V[i] - this->timestamps_V[0])/ (float) (this->timestamps_V[this->timestamps_V.size()-1] - this->timestamps_V[0]);
                glLineWidth(2.0);
                glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
                glBegin(GL_QUADS);
                glVertex3f(-5.0f + 10*percent_x_ts - 0.004, -4.0f, 0.0f);
                glVertex3f(-5.0f + 10*percent_x_ts - 0.004, 4.0f, 0.0f);
                glVertex3f(-5.0f + 10*percent_x_ts + 0.004, 4.0f, 0.0f);
                glVertex3f(-5.0f + 10*percent_x_ts + 0.004, -4.0f, 0.0f);
                glColor3f(1.0f,1.0f,1.0f);
                glEnd();
            }


    }
    else {
        qWarning( "No valid results !!");
    }



    glFlush();
    std::cout << "[END] PAINT " << std::endl;
}



void EntropyViewer::fetch_data (QString host, QString name, QString user, QString pass)
{


    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "entropies_mysql_connection");
    db.setHostName(host);
    db.setDatabaseName(name);
    db.setUserName(user);
    db.setPassword(pass);
    //db.transaction();

    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";







    std::cout << " [BEGIN] Fetch data" << std::endl;



    QSqlQuery q;

    // TCP

    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='6' AND type_of_hist='1' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            timestamps_V.push_back(q.value(0).toInt());
            tcp_sip_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 6 - 1");
    }

    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='6' AND type_of_hist='2' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            tcp_sport_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 6 - 2");
    }

    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='6' AND type_of_hist='3' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            tcp_dip_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 6 - 3");
    }

    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='6' AND type_of_hist='4' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            tcp_dport_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 6 - 4");
    }

    q = db.exec ( "SELECT timestamp,n_basis,log(1+score) FROM anomaly_score ORDER BY timestamp ASC, n_basis ASC" );
    if ( q.size()>0) {
        q.seek(-1);


        for ( int i = 0; i < 10; i++ ) {
            anomalies_V.push_back ( std::vector<float>() );

            for ( int j = 0; j < q.size()/10; j++ )
              anomalies_V[i].push_back ( 0.0f );
          }

        int col=0;
        while ( q.next() ) {

            //unsigned long ts = q.value(0).toInt();
            unsigned long nb = q.value(1).toInt();
            float score = q.value(2).toFloat();


            anomalies_V[nb-1][col]= score;

            if (nb == 10)
                col++;


        }
    }
    else {
        qWarning("Warning: empty table with anomaly");
    }








    // UDP


    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='17' AND type_of_hist='1' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            udp_sip_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 17 - 1");
    }

    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='17' AND type_of_hist='2' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            udp_sport_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 17 - 2");
    }

    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='17' AND type_of_hist='3' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            udp_dip_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 17 - 3");
    }

    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='17' AND type_of_hist='4' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            udp_dport_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 17 - 4");
    }










    // ICMP

    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='1' AND type_of_hist='1' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            icmp_sip_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 1 - 1");
    }


    q = db.exec ( "SELECT timestamp,entropy FROM entropies WHERE proto='1' AND type_of_hist='3' ORDER BY timestamp ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            icmp_dip_V.push_back(q.value(1).toFloat());
        }
    }
    else {
        qWarning("Warning: empty table with 1 - 3");
    }



    q = db.exec ( "SELECT eigenvalue FROM eigenvalues ORDER BY n_basis ASC" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            eigenvalues_V.push_back(q.value(0).toDouble());
        }
    }
    else {
        qWarning("Warning: empty table with eigenvalues");
    }

    q = db.exec ( "SELECT DISTINCT snortname FROM snortlog" );
    if ( q.size()>0) {
        q.seek(-1);
        while ( q.next() ) {
            anomaly_snort_name.push_back(q.value(0).toString().toStdString());
        }
    }
    else {
        qWarning("Warning: empty table with eigenvalues");
    }


    for (size_t i = 0; i<anomaly_snort_name.size(); i++)
    {
        QAction * newAct = new QAction(QString(this->anomaly_snort_name[i].c_str()), this);
        newAct->setCheckable(true);
        newAct->setChecked(false);
        this->anomaly_snort_action.push_back(newAct);
        this->anomaly_snort_ischeck.push_back(false);
    }



    this->snort_anomalies_matrix.assign(anomaly_snort_name.size(), std::vector<float>());

    for (size_t i = 0; i<anomaly_snort_name.size(); i++)
    {

        qWarning() << QString("SELECT timestamp, COUNT(*) FROM snortlog WHERE snortname='") + QString(anomaly_snort_name[i].c_str()) + QString("' GROUP BY timestamp;");

        q = db.exec ( QString("SELECT timestamp, COUNT(*) FROM snortlog WHERE snortname='") + QString(anomaly_snort_name[i].c_str()) + QString("' GROUP BY timestamp;") );



        if ( q.size()>0)
        {
            q.seek(-1);

            this->snort_anomalies_matrix[i].assign(timestamps_V.size(), 0.0f);

            while ( q.next() )
            {
                int index = (q.value(0).toInt()-timestamps_V[0])/(timestamps_V[1]-timestamps_V[0]);
                if (index < 0)
                    qWarning() << "ERRROOORRRRRRRR on the index" << index << q.value(0).toInt() << timestamps_V[0];
                else
                    this->snort_anomalies_matrix[i][index] = q.value(1).toFloat();
            }
        }
        else {
            qWarning("Warning: empty table with eigenvalues");
        }
    }



    db.close();


    std::cout << "[END] Fetch data: lines = " << timestamps_V.size() << std::endl;



}


void EntropyViewer::compute_timestamps_and_event()
{

    init_timestamp_click = -1;
    end_timestamp_click = -1;
    int len_hist = timestamps_V.size();


    if ( len_hist <= 1)
        return;



    float percent_x = (float) pressPos.x() / (float) this->width;
    int current_bin_click = round((float)len_hist * percent_x);
    init_timestamp_click = timestamps_V[current_bin_click];


    percent_x = (float) releasePos.x() / (float) this->width;
    current_bin_click = round((float)len_hist * percent_x);
    end_timestamp_click = timestamps_V[current_bin_click];


    if (init_timestamp_click>end_timestamp_click) {
        int temp = end_timestamp_click;
        end_timestamp_click = init_timestamp_click;
        init_timestamp_click = temp;
    }


    emit time_click(init_timestamp_click, end_timestamp_click, show_tcp_sport, show_tcp_dport, show_tcp_sip, show_tcp_dip, \
                    show_udp_sport, show_udp_dport, show_udp_sip, show_udp_dip, \
                    show_icmp_sip, show_icmp_dip);



    qWarning() << init_timestamp_click << end_timestamp_click;


    QString txt = "SELECTION: ";
    txt += ctime(((time_t*) &init_timestamp_click));
    txt[txt.size()-1] = ' ';

    if (init_timestamp_click != end_timestamp_click) {
        txt += " -> ";
        txt += ctime(((time_t*) &end_timestamp_click));
        txt[txt.size()-1] = ' ';
    }
    emit this->send_log(txt);


    ts->set_new_timestamp(init_timestamp_click,end_timestamp_click);




    repaint();

}



void EntropyViewer::anomaly_selected(int begin, int end)
{
    int w = this->width;

    int t0 = this->timestamps_V[0];
    int t1 = this->timestamps_V[this->timestamps_V.size()-1];

    double x_begin = (double)w * (double)(begin-t0) / (double)(t1-t0) ;
    double x_end   = (double)w * (double)(end-t0)   / (double)(t1-t0) ;

    pressPos.setX(floor(x_begin));
    releasePos.setX(floor(x_end));


    compute_timestamps_and_event();

}


void EntropyViewer::mouseMoveEvent(QMouseEvent *event)
{
    actualPos = event->pos();
    repaint();
}


void EntropyViewer::time_backward()
{
    int ticks = this->timestamps_V.size();
    int delta_T_pixel = this->width/ticks;
    releasePos.setX(releasePos.x()-delta_T_pixel);
    pressPos.setX(pressPos.x()-delta_T_pixel);
    compute_timestamps_and_event();
}


void EntropyViewer::time_forward()
{
    int ticks = this->timestamps_V.size();
    int delta_T_pixel = this->width/ticks;
    releasePos.setX(releasePos.x()+delta_T_pixel);
    pressPos.setX(pressPos.x()+delta_T_pixel);
    compute_timestamps_and_event();
}

void EntropyViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        releasePos = event->pos();
        repaint();
        qWarning() << "REL";
        compute_timestamps_and_event();
        this->is_mouse_button_pressed = false;
    }
}

void EntropyViewer::mousePressEvent(QMouseEvent *event)
{

    if ( (event->type() == QEvent::MouseButtonPress) && !(event->buttons() == Qt::RightButton) && !(event->modifiers()==Qt::ControlModifier) )
    {
        this->is_mouse_button_pressed = true;
        releasePos = event->pos();
        pressPos = event->pos();
        qWarning() << "PRESS";
        repaint();
    }

    if ( (event->type() == QEvent::MouseButtonPress) && (event->modifiers() == Qt::RightButton || event->modifiers() == Qt::ControlModifier))
    {
        qWarning() << "right button!";

        QPoint globalpos= event->pos();

        menu->clear();

        for (size_t i = 0; i<anomaly_snort_name.size(); i++)
        {
            QAction * act = this->anomaly_snort_action[i];
            act->setChecked(this->anomaly_snort_ischeck[i]);
            menu->addAction(act);

        }

        menu->popup(mapToGlobal(globalpos));
        menu->show();

    }

}


void EntropyViewer::anomaly_menu_triggered ( QAction * action )
{

    QString item_selected = action->text();

    for (size_t i = 0; i<anomaly_snort_name.size(); i++)
    {
        if (item_selected == QString(anomaly_snort_name[i].c_str()) )
        {
            this->anomaly_snort_ischeck[i] = (this->anomaly_snort_ischeck[i]==true? false:true);
            qWarning() << item_selected << this->anomaly_snort_ischeck[i];
        }
    }


    repaint();


}





void EntropyViewer::visualization_tcp_sport (int newState) {
    show_tcp_sport = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "tcp_sport new state = " << newState << std::endl;
    repaint();
}

void EntropyViewer::visualization_tcp_dport (int newState) {
    show_tcp_dport = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "tcp_dport new state = " << newState << std::endl;
    repaint();
}
void EntropyViewer::visualization_tcp_sip (int newState) {
    show_tcp_sip = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "tcp_sip new state = " << newState << std::endl;
    repaint();
}
void EntropyViewer::visualization_tcp_dip (int newState) {
    show_tcp_dip = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "tcp_dip new state = " << newState << std::endl;
    repaint();
}

void EntropyViewer::visualization_udp_sport (int newState) {
    show_udp_sport = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "udp_sport new state = " << newState << std::endl;
    repaint();
}

void EntropyViewer::visualization_udp_dport (int newState) {
    show_udp_dport = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "udp_dport new state = " << newState << std::endl;
    repaint();
}
void EntropyViewer::visualization_udp_sip (int newState) {
    show_udp_sip = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "udp_sip new state = " << newState << std::endl;
    repaint();
}
void EntropyViewer::visualization_udp_dip (int newState) {
    show_udp_dip = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "udp_dip new state = " << newState << std::endl;
    repaint();
}

void EntropyViewer::visualization_icmp_sip (int newState) {
    show_icmp_sip = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "icmp_sip new state = " << newState << std::endl;
    repaint();
}
void EntropyViewer::visualization_icmp_dip (int newState) {
    show_icmp_dip = (newState==0) ? false: true;
    compute_timestamps_and_event();
    std::cout<< "icmp_dip new state = " << newState << std::endl;
    repaint();
}
void EntropyViewer::visualization_anomaly (int newState) {
    show_anomaly = (newState==0) ? false: true;
    compute_anomaly_list();
    repaint();
}
void EntropyViewer::visualization_n_basis (int newState) {
    n_basis = newState-1;
    compute_anomaly_list();
    compute_energy_eigenvalues();

    repaint();
}


void EntropyViewer::compute_energy_eigenvalues()
{
    float p = 0;
    float total = 0.0f;

    for (size_t i = 0; i < eigenvalues_V.size(); i++)
    {
        total += (eigenvalues_V[i]);
        if ((int)i == n_basis) {
            p = total;
        }
    }

    if (total > 0) {
        p = p/total*100.0f;
        emit energy_eigen_signal(p);
    }
}



float EntropyViewer::compute_anomaly_max() {


    float fmax = 0.0f;

    for (size_t i=0; i< anomalies_V[n_basis].size(); i++){
            if (anomalies_V[n_basis][i] > fmax)
                fmax = anomalies_V[n_basis][i];
        }

    return fmax;

}


void EntropyViewer::compute_anomaly_list()
{

    double T = this->compute_anomaly_max()*5.0f/100.0f;

    AnomalyList * alist = new AnomalyList();
    bool inside_anomaly = false;

    for (size_t i=0; i< anomalies_V[n_basis].size(); i++)
    {
        if ( (anomalies_V[n_basis][i] > T) && (inside_anomaly == false) )
        {
            // begin of a new anomaly
            alist->init_anomaly_timestamp.push_back(timestamps_V[i-1]);
            alist->intensity.push_back((double) anomalies_V[n_basis][i]);
            alist->end_anomaly_timestamp.push_back(timestamps_V[i]);
            inside_anomaly = true;
        }

        else if ( (anomalies_V[n_basis][i] < T) & (inside_anomaly == true) )
        {
            // end of ananomaly
            alist->end_anomaly_timestamp[alist->end_anomaly_timestamp.size()-1] = timestamps_V[i];
            inside_anomaly = false;
        }

        else if ( (anomalies_V[n_basis][i] > T) && (inside_anomaly == true) )
        {
            // inside an anomaly
            alist->intensity[alist->intensity.size()-1] += anomalies_V[n_basis][i] ;
        }


    }

   emit anomaly_list_signal(alist);
   compute_energy_eigenvalues();



}


float EntropyViewer::cerca_max_value()
{
    float fmax = 0;

    if (show_tcp_sport == true)
        for (size_t i=0; i< tcp_sport_V.size(); i++){
            if (tcp_sport_V[i] > fmax)
                fmax = tcp_sport_V[i];
        }
    if (show_tcp_dport == true)
        for (size_t i=0; i< tcp_dport_V.size(); i++){
            if (tcp_dport_V[i] > fmax)
                fmax = tcp_dport_V[i];
        }
    if (show_tcp_sip == true)
        for (size_t i=0; i< tcp_sip_V.size(); i++){
            if (tcp_sip_V[i] > fmax)
                fmax = tcp_sip_V[i];
        }
    if (show_tcp_dip == true)
        for (size_t i=0; i< tcp_dip_V.size(); i++){
            if (tcp_dip_V[i] > fmax)
                fmax = tcp_dip_V[i];
        }

    if (show_udp_sport == true)
        for (size_t i=0; i< udp_sport_V.size(); i++){
            if (udp_sport_V[i] > fmax)
                fmax = udp_sport_V[i];
        }
    if (show_udp_dport == true)
        for (size_t i=0; i< udp_dport_V.size(); i++){
            if (udp_dport_V[i] > fmax)
                fmax = udp_dport_V[i];
        }
    if (show_udp_sip == true)
        for (size_t i=0; i< udp_sip_V.size(); i++){
            if (udp_sip_V[i] > fmax)
                fmax = udp_sip_V[i];
        }
    if (show_udp_dip == true)
        for (size_t i=0; i< udp_dip_V.size(); i++){
            if (udp_dip_V[i] > fmax)
                fmax = udp_dip_V[i];
        }

    if (show_icmp_sip == true)
        for (size_t i=0; i< icmp_sip_V.size(); i++){
            if (icmp_sip_V[i] > fmax)
                fmax = icmp_sip_V[i];
        }
    if (show_icmp_dip == true)
        for (size_t i=0; i< icmp_dip_V.size(); i++){
            if (icmp_dip_V[i] > fmax)
                fmax = icmp_dip_V[i];
        }
    return fmax;

}


void EntropyViewer::selected_histograms(std::vector<int> timestamps)
{
    this->selected_histograms_V = timestamps;

    qWarning() << timestamps.size() << "       DDDD    " << this->selected_histograms_V.size();

   repaint();
}
