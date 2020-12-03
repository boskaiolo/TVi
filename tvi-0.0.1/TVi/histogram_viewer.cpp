#include "histogram_viewer.h"
#include <QtGui>
#include <QtOpenGL>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtDebug>
#include <QProgressDialog>
#include <string>
#include <sstream>
#include <iostream>
#include <GLUT/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>

#include "db_credential.h"
#include "colors.h"
#include "tools.h"
#include "pixel2gl.h"
#include "well_known_ports.h"



#define EPSILON 0.0

#define GLUT_FONT GLUT_BITMAP_8_BY_13
#define GLUT_FONT_HEIGHT 14


// Initialization, clear vectors, open the db and fetch timestamp data
HistogramViewer::HistogramViewer(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    db = QSqlDatabase::addDatabase("QMYSQL", "hist_mysql_connection");
    db.setHostName(DB_host);
    db.setDatabaseName(DB_name);
    db.setUserName(DB_user);
    db.setPassword(DB_pass);
   // db.transaction();

    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";

    log_hist = false;

    this->clear_vectors();
    this->fetch_timestamp_vector();

    selectBuf = (GLuint*) malloc(sizeof(GLuint) * timestamp_V.size() * 12);
    bufsize = sizeof(GLuint) * timestamp_V.size() * 12;


    table_tcp_sip.name = "TCP - SIP";
    table_tcp_sport.name = "TCP - SPORT";
    table_tcp_dip.name = "TCP - DIP";
    table_tcp_dport.name = "TCP - DPORT";

    table_udp_sip.name = "UDP - SIP";
    table_udp_sport.name = "UDP - SPORT";
    table_udp_dip.name = "UDP - DIP";
    table_udp_dport.name = "UDP - DPORT";

    table_icmp_sip.name = "ICMP - SIP";
    table_icmp_dip.name = "ICMP - DIP";



    this->setMouseTracking(true);
    is_selection_mode_opengl = false;
    this->matrix_timestamp_clicked.clear();
    this->timestamp_selected_V.clear();


    connect(this, SIGNAL(repaint_me()), this,SLOT(paint_all()));

    this->setFocusPolicy( Qt::StrongFocus);
    this->setEnabled(true);
    this->one_global_histogram = false;

}


// Clean of the vectors
HistogramViewer::~HistogramViewer()
{
    timestamp_V.clear();
    clear_vectors();
    free(selectBuf);
}


// Insert here all the elements to be cleaned!
void HistogramViewer::clear_vectors()
{
    table_tcp_sip.ClearAll();
    table_tcp_sport.ClearAll();
    table_tcp_dip.ClearAll();
    table_tcp_dport.ClearAll();

    table_udp_sip.ClearAll();
    table_udp_sport.ClearAll();
    table_udp_dip.ClearAll();
    table_udp_dport.ClearAll();

    table_icmp_sip.ClearAll();
    table_icmp_dip.ClearAll();
}



void HistogramViewer::keyPressEvent(QKeyEvent *event)
{
    int keypressed = event->key();

    if (keypressed == Qt::Key_Space)
    {
        this->one_global_histogram=!this->one_global_histogram;

        qWarning() << "global hist set to " << this->one_global_histogram;
        repaint();
    }

}




// Slot: checkbox of the LOG hist clicked
void HistogramViewer::log_hist_clicked(bool showlog_status) {
    log_hist = showlog_status;
    repaint();
}


// Slot: The user click on the entropy graph (2 timestamp + 10 features status)
void HistogramViewer::time_clicked(int timestamp_init, int timestamp_end, \
                                   bool tcp_sport, bool tcp_dport, bool tcp_sip, bool tcp_dip, \
                                   bool udp_sport, bool udp_dport, bool udp_sip, bool udp_dip, \
                                   bool icmp_sip, bool icmp_dip) {

    clear_vectors();


    this->timestamp_init = timestamp_init;
    this->timestamp_end = timestamp_end;


    table_tcp_sip.setStatus(tcp_sip);
    table_tcp_sport.setStatus(tcp_sport);
    table_tcp_dip.setStatus(tcp_dip);
    table_tcp_dport.setStatus(tcp_dport);

    table_udp_sip.setStatus(udp_sip);
    table_udp_sport.setStatus(udp_sport);
    table_udp_dip.setStatus(udp_dip);
    table_udp_dport.setStatus(udp_dport);

    table_icmp_sip.setStatus(icmp_sip);
    table_icmp_dip.setStatus(icmp_dip);

    fetch_histograms(timestamp_init, timestamp_end);

    repaint();

}


// Fetch the timestamps in the db (each timestamp has a proper features table!!)
void HistogramViewer::fetch_timestamp_vector()
{
    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";

    QSqlQuery q;

    q = db.exec ( "SELECT timestamp FROM timestamp_list;" );
        if ( q.size()>0) {
            q.seek(-1);
            while ( q.next() ) {
                timestamp_V.push_back(q.value(0).toInt());
            }
        }
        else {
            qWarning("Warning: histogram empty table with 6 - 1");
        }
}



// Fetch the histogram and insert in the RowsTable
void HistogramViewer::fetch_table(std::string query_where, RowsTable * table, int timestamp_init, int timestamp_end, bool isPort, bool isIP, std::string errorbuffer, QProgressDialog * progress = NULL)
{

    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";
    QSqlQuery q;


    for (size_t i = 0; i<timestamp_V.size(); i++)
    {
        if (timestamp_V[i] == timestamp_init)
            timestamp_init_index = i;

        if (timestamp_V[i] == timestamp_end)
            timestamp_end_index = i;

    }


    table->isIP = isIP;
    table->isPort = isPort;

    std::string string1;
    std::string string2 = " ORDER BY counter DESC";
    std::string querystr;


    if (log_hist==false)
        string1 = "SELECT bin, counter FROM hist_";
    else
        string1 = "SELECT bin, log(1+counter) FROM hist_";

    for (size_t i= timestamp_init_index; i<= timestamp_end_index; i++) {
        querystr.clear();
        querystr = string1 + int2stdstr(timestamp_V[i]) +  query_where + string2;
        //std::cout << querystr << std::endl;

        q = db.exec ( querystr.c_str());
        if ( q.size()>0) {
            q.seek(-1);
            int c=0;
            while ( q.next() ) {
                table->InsertValues(q.value(0).toString().toStdString(), q.value(1).toDouble(), i - timestamp_init_index, c);
                c++;
            }

            table->PlotTables();

        }
        else {
            qWarning() << "Warning: histogram empty!!! table with" << errorbuffer.c_str();
        }
    }
    table->PrintSize();


    if (progress != NULL)
    {
        progress->setValue(progress->value()+1);
    }


}


// Fetch the histograms of the features for every timestamp between init and end
void HistogramViewer::fetch_histograms(int timestamp_init, int timestamp_end)
{


    if (this->isHidden() == true)
        return;

    QProgressDialog * progress = new QProgressDialog(this);
    progress->setLabelText("Fetching data from MySql DB");
    progress->setRange(0, this->get_n_active_features());
    progress->setMinimumDuration (1);
    progress->setValue(0);
    progress->setModal(true);

    // TCP

    if (table_tcp_sip.isActive())
        fetch_table(" WHERE proto='6' AND type_of_hist='1' ", &table_tcp_sip, timestamp_init, timestamp_end, false, true, " 6 - 1", progress);
    if (table_tcp_sport.isActive())
        fetch_table(" WHERE proto='6' AND type_of_hist='2' ", &table_tcp_sport, timestamp_init, timestamp_end, true, false, " 6 - 2", progress);
    if (table_tcp_dip.isActive())
        fetch_table(" WHERE proto='6' AND type_of_hist='3' ", &table_tcp_dip, timestamp_init, timestamp_end, false, true, " 6 - 3", progress);
    if (table_tcp_dport.isActive())
        fetch_table(" WHERE proto='6' AND type_of_hist='4' ", &table_tcp_dport, timestamp_init, timestamp_end, true, false, " 6 - 4", progress);


    // UDP

    if (table_udp_sip.isActive())
        fetch_table(" WHERE proto='17' AND type_of_hist='1' ", &table_udp_sip, timestamp_init, timestamp_end, false, true, " 17 - 1", progress);
    if (table_udp_sport.isActive())
        fetch_table(" WHERE proto='17' AND type_of_hist='2' ", &table_udp_sport, timestamp_init, timestamp_end, true, false, " 17 - 2", progress);
    if (table_udp_dip.isActive())
        fetch_table(" WHERE proto='17' AND type_of_hist='3' ", &table_udp_dip, timestamp_init, timestamp_end, false, true, " 17 - 3", progress);
    if (table_udp_dport.isActive())
        fetch_table(" WHERE proto='17' AND type_of_hist='4' ", &table_udp_dport, timestamp_init, timestamp_end, true, false, " 17 - 4", progress);

    // ICMP

    if (table_icmp_sip.isActive())
        fetch_table(" WHERE proto='1' AND type_of_hist='1' ", &table_icmp_sip, timestamp_init, timestamp_end, false, true, " 1 - 1", progress);
    if (table_icmp_dip.isActive())
        fetch_table(" WHERE proto='1' AND type_of_hist='3' ", &table_icmp_dip, timestamp_init, timestamp_end, false, true, " 1 - 3", progress);

    progress->~QProgressDialog();


}


void HistogramViewer::initializeGL()
{
    std::cout << "[BEGIN] Init" << std::endl;

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

    glTranslatef(0.0f, 0.0f, -GL_H/2);


    if (GL_H != GL_W) {
        qFatal("[histogram_viewer] look at the define: GL_H and GL_W MUST BE EQUAL!!!!");
    }

    std::cout << "[END] Init " << std::endl;
 }

void HistogramViewer::resizeGL(int width, int height)
 {
    this->width = width;
    this->height = height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    std::cout << "W = " << width << "  H = " << height << std::endl;
 }





float HistogramViewer::click_sorted_port_hist(RowsTable * table, float click_x, float click_y)
{



    int n_bins = 65535;
    float lenhist = log(n_bins+1.0);

    float w_scale = table->w2/lenhist;


    float sup = exp((click_x-table->x2-EPSILON)/(w_scale)) - 1;
    float inf = exp((click_x-table->x2)/(w_scale)) - 2;


    int bin = ceil(inf);

    if (bin < 0)
        return bin;


    int count = 0;


    for (int i=0; i<table->GetNumCols(); i++)
    {
        if ( atoi(table->GetBinValue(0, i).c_str()) == bin)
        {
            count = (int) floor((log_hist==true? exp(0.1+table->GetCounterValue(0, i)): 0.1+table->GetCounterValue(0, i)));
            break;
        }
    }

    qWarning() << "CLICK ON PORT " << bin << port2service(bin).c_str();
    qWarning() << sup << inf;

    QString empty = " ";
    QString s = "CLICK ON PORT:" + empty + int2str(bin);
    s += empty + port2service(bin).c_str() + empty + " # count = " + empty + int2str(count);
    emit this->send_log(s);



    return bin;
}



float HistogramViewer::mouse_over_unsorted_hist(RowsTable * table, float click_x, float click_y)
{

    int n_bins = table->GetNumCols();


    float lenhist = log_hist==true? log((float)n_bins): (float) n_bins-1;
    float w_scale = table->w1/lenhist;


    float percentage_w = (click_x-table->x1)/w_scale;


    int bin;

    if (log_hist == true)
    {
        bin = ceil(exp(percentage_w) - 2);
    }
    else
    {
        bin = floor(percentage_w);
    }


    std::cout << bin <<  std::endl;



    if (bin < 0 || bin > n_bins)
        return bin;

    table->clear_selected_cells();
    table->insert_new_selected_cell(0, bin);

    return bin;

}



float HistogramViewer::mouse_over_sorted_port_hist(RowsTable * table, float click_x, float click_y)
{

    int n_bins = 65535;
    float lenhist = log(n_bins+1.0);

    float w_scale = table->w2/lenhist;


    //float sup = exp((click_x-table->x2-EPSILON)/(w_scale)) - 1;
    float inf = exp((click_x-table->x2)/(w_scale)) - 2;


    int bin = ceil(inf);

    if (bin < 0)
        return bin;

    for (int i=0; i<table->GetNumCols(); i++)
    {
        if ( atoi(table->GetBinValue(0, i).c_str()) == bin)
        {
            table->clear_selected_cells();
            table->insert_new_selected_cell(0, i);
            break;
        }
    }


    return bin;
}


void HistogramViewer::paintGL_sorted_port_hist(RowsTable * table, float x, float y, float h, float w, float r, float g, float b)
{

    float x_init = x;
    float y_init = y;

    int pixel_Yaxeslabel_w = glutBitmapLength(GLUT_FONT, (const unsigned char*)float2stdstr(table->GetMAXCounter()).c_str());
    int pixel_Yaxeslabel_h = GLUT_FONT_HEIGHT;


    float gl_x, gl_y;
    qt2gl_coord(this->width, this->height, GL_H, GL_W, pixel_Yaxeslabel_w, pixel_Yaxeslabel_h, &gl_x, &gl_y);


    float w_gl_timestamp = gl_x+GL_W/2;
    if (w_gl_timestamp > w)
        w_gl_timestamp = w;


    x += (w_gl_timestamp);
    w-= 1.2*(w_gl_timestamp);


    y += 0.02*h;
    h -= 2*(0.02*h);




    int n_bins = 65535;
    float lenhist = log(n_bins+1);

    float max_counter = table->GetMAXCounter();

    float w_scale = w/lenhist;
    float h_scale = h/max_counter;


    glColor3f(r,g,b);

    for (int i=0; i<table->GetNumCols(); i++) {

       int port = atoi(table->GetBinValue(0,i).c_str());

       if (table->is_cell_selected(0, i))
       {
           glColor3f(1.0f,1.0f,1.0f);
           glLineWidth(1.5f);
       }

       glBegin(GL_QUADS);
       glVertex3f(x+EPSILON+log(port+1)*w_scale, y, 0.0f);
       glVertex3f(x+EPSILON+log(port+1)*w_scale, y+table->GetCounterValue(0,i)*h_scale, 0.0f);
       glVertex3f(x+log(port+2)*w_scale, y+table->GetCounterValue(0,i)*h_scale, 0.0f);
       glVertex3f(x+log(port+2)*w_scale, y, 0.0f);
       glEnd();

       glBegin(GL_LINES);
       glVertex3f(0.5*(x+EPSILON+log(port+1)*w_scale + x+log(port+2)*w_scale), y, 0.0f);
       glVertex3f(0.5*(x+EPSILON+log(port+1)*w_scale + x+log(port+2)*w_scale), y+table->GetCounterValue(0,i)*h_scale, 0.0f);
       glEnd();

       if (table->is_cell_selected(0, i))
       {
           glLineWidth(1.0f);
           QString s = "PORT " + QString(table->GetBinValue(0,i).c_str()) + " -> # " + QString(float2stdstr(table->GetCounterValue(0,i)).c_str());
           renderBitmapString(s.toStdString(), x+log(port+2)*w_scale, y+0.05+table->GetCounterValue(0,i)*h_scale);
           glColor3f(r,g,b);
       }

    }




    renderBitmapString(int2stdstr(21), x+EPSILON+log(21+1)*w_scale, y_init);
    renderBitmapString(int2stdstr(53), x+EPSILON+log(53+1)*w_scale, y_init);
    renderBitmapString(int2stdstr(80), x+EPSILON+log(80+1)*w_scale, y_init);
    renderBitmapString(int2stdstr(1024), x+EPSILON+log(1024+1)*w_scale, y_init);

    renderBitmapString(float2stdstr(max_counter), x_init, y+h);

    glLineWidth(1.0f);


    // draw the axes
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x+w, y, 0.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x, y+h, 0.0f);
    glEnd();


    table->setPositionOnScreen_2nd_graph(x,y,w,h);



}





void HistogramViewer::paintGL_Hilbert_map_ip(RowsTable * table, float x, float y, float h, float w)
{



    h = 0.95*h;
    w = 0.95*w;
    x = x+0.02*w;
    y = y+0.02*h;




    // IP = ip1.ip2.ip3.ip4, where ip{1,2,3,4} are integers
    int ip1;
    int ip2;
    int ip3;
    int ip4;




    float max_counter = table->GetMAXCounter();
    float sat_scale = 1.0f/max_counter;



    int depth = 12;


    for (int i = table->GetNumCols()-1; i>=0; i--) {
        sscanf(table->GetBinValue(0,i).c_str(), "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
        unsigned long longip = ip4 + ip3*pow(2,8) + ip2*pow(2,16) + ip1*pow(2,24);
        double in = ((double) longip)/(pow(2,32)-1.0) * (1.0-pow(4.0,-1.0*depth));
        double xout, yout;

        HilbertMap(in, &xout, &yout, depth, 0);

        //std::cout << table->GetBinValue(0,i) << " " << longip << "       " << table->GetCounterValue(0,i)<< std::endl;
        //std::cout << xout << "\t\t" << yout << std::endl;


        float graylevel = sat_scale*table->GetCounterValue(0,i);
        glColor4f(graylevel,graylevel,graylevel, 0.8f);

        glPointSize(4.0f);
        glBegin(GL_POINTS);
        glVertex3f(x+xout*w, y+yout*h, 0.0f);
        glEnd();
        glPointSize(1.0f);

        //std::cout << x+xout*w << "\t\t" << y+yout*h << std::endl << std::endl;


    }

    // draw the axes
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x+w, y, 0.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x, y+h, 0.0f);
    glEnd();


    table->setPositionOnScreen_2nd_graph(x,y,w,h);

}




void HistogramViewer::renderBitmapString(std::string txt, float x, float y)
{
    glRasterPos3f(x, y,0);
    for (size_t c = 0; c<txt.length(); c++) {
      glutBitmapCharacter(GLUT_FONT, txt[c]);
    }
}





void HistogramViewer::paintGL_hist_vector_matrix(RowsTable * table, float x, float y, float h, float w, float r, float g, float b)
{


    x += 0.01*w;
    y += 0.01*h;

    h -= 0.02*h;
    w -= 0.02*w;

    double angle_c_rad = atan(5.0*w/(8.0*h));
    //double angle_c_deg = angle_c_rad*180.0/PI;

    // draw the axes
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);
    glVertex3f(x + w/2 , y+(0.8f*h), 0.0f);
    glVertex3f(x + w/2 , y+(0.95f*h), 0.0f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(x + w/2 -0.003*w, y+(0.94f*h), 0.0f);
    glVertex3f(x + w/2 +0.003*w, y+(0.94f*h), 0.0f);
    glVertex3f(x + w/2 , y+(0.95f*h), 0.0f);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(x + w/2 , y+(0.8f*h), 0.0f);
    glVertex3f(x + w*0.95f , y+(0.8f*h), 0.0f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(x + w*0.94f , y+(0.803f*h), 0.0f);
    glVertex3f(x + w*0.94f , y+(0.797f*h), 0.0f);
    glVertex3f(x + w*0.95f , y+(0.8f*h), 0.0f);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(x + w/2 , y+(0.8f*h), 0.0f);
    glVertex3f(x + w/2 - h*0.8*tan(angle_c_rad) , y, 0.0f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x, y+(0.005*h), 0.0f);
    glVertex3f(x+0.004*w, y, 0.0f);
    glEnd();



    double length_L = 4.0/5.0*h/cos (angle_c_rad);

    int n_bins = table->GetNumCols();
    float lenhist = (log_hist==true? log((float)n_bins+1): (float) n_bins);

    float max_counter = table->GetMAXCounter();

    float bin_scale = 0.45f*w/lenhist;
    float h_scale = 0.15f*h/max_counter;




    for (int c=0; c<table->GetNumRows(); c++)
    {

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glPushName(c);


        // axes of the hist

        glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
        glLineWidth(0.5f);
        glBegin(GL_LINES);
        glVertex3f(x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);
        glVertex3f(x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad)+0.45f*w, y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);
        glEnd();


        bool is_clicked = false;
        for (size_t i=0; i<this->matrix_timestamp_clicked.size(); i++)
        {
            if (this->matrix_timestamp_clicked[i] == c)
                is_clicked = true;
        }

        if ( ! is_clicked)
        {
            glColor3f(r,g,b);
            glLineWidth(2.0f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glLineWidth(3.0f);
        }

        glBegin(GL_LINE_STRIP);
        glVertex3f(x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);

        int i;
        for (i=0; i<n_bins; i++)
        {
            if (table->GetCounterValue(c,i) < 1E-8)
                break;

            if (log_hist == false)
                glVertex3f(i*bin_scale + x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), h_scale *table->GetCounterValue(c,i) + y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);
            else
                glVertex3f(log(i+1)*bin_scale + x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), h_scale *table->GetCounterValue(c,i) + y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);

        }

        i--;
        if (log_hist == false)
            glVertex3f(i*bin_scale + x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);
        else
            glVertex3f(log(i+1)*bin_scale + x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);

        i++;

        glEnd();




        if (is_clicked)
        {
            std::string s = QString(ctime((time_t *) &timestamp_V[c])).toStdString();
            s[s.size()-1] = '\0';

            this->renderBitmapString(s, x, h_scale *table->GetCounterValue(c,0) + y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad));
        }


        // final line tick
        if (i>0)
        {

            glBegin(GL_LINES);
            if (log_hist == false)
            {
                glVertex3f((i-1)*bin_scale + x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), (0.005*h) + y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);
                glVertex3f((i-1)*bin_scale + x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), (-0.005*h) + y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);
            }
            else
            {
                glVertex3f(log(i)*bin_scale + x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), (0.005*h) + y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);
                glVertex3f(log(i)*bin_scale + x+w/2 - c*(length_L/table->GetNumRows())*sin(angle_c_rad), (-0.005*h) + y+0.8f*h - c*(length_L/table->GetNumRows())*cos(angle_c_rad), 0.0f);
            }
            glEnd();
        }


        glPopName();
        glPopMatrix();


    }




    int max_val = int(0.1 + (log_hist==true? log((float)n_bins+1): table->GetMAXCounter()));
    renderBitmapString(int2stdstr(max_val), x + w/2 , y+(0.95f*h));
    renderBitmapString("time", x+0.01*w , y);
    renderBitmapString(int2stdstr(table->GetNumCols()), x + w*0.95f , y+(0.8f*h));


}


void HistogramViewer::paintGL_hist_vector(RowsTable * table, float x, float y, float h, float w, float r, float g, float b)
{

    float x_init = x;
    //float y_init = y;


    int pixel_Yaxeslabel_w = glutBitmapLength(GLUT_FONT, (const unsigned char*)float2stdstr(table->GetMAXCounter()).c_str());
    int pixel_Yaxeslabel_h = GLUT_FONT_HEIGHT;


    float gl_x, gl_y;
    qt2gl_coord(this->width, this->height, GL_H, GL_W, pixel_Yaxeslabel_w, pixel_Yaxeslabel_h, &gl_x, &gl_y);


    float w_gl_timestamp = gl_x+GL_W/2;
    if (w_gl_timestamp > w)
        w_gl_timestamp = w;

    //float h_gl_timestamp = -gl_y+GL_H/2;

    x += (w_gl_timestamp);
    w-= 1.2*(w_gl_timestamp);


    y += 0.02*h;
    h -= 2*(0.02*h);


    int n_bins = table->GetNumCols();


    float lenhist = log_hist==true? log((float)n_bins): (float) n_bins-1;

    float max_counter = table->GetMAXCounter();

    float w_scale = w/lenhist;
    float h_scale = h/max_counter;


    QString aLabel;
    float aLabel_x, aLabel_y;
    bool is_aLabalActive = false;

    glColor3f(r,g,b);
    glLineWidth(1.4f);

    for (int c=0; c<table->GetNumRows(); c++)
    {
        glBegin(GL_LINE_STRIP);

        for (int i=0; i<n_bins; i++)
        {
            if (log_hist == false)
            {
                if (table->is_cell_selected(0, i))
                {
                    glColor3f(1.0f,1.0f,1.0f);
                    glLineWidth(4.0f);
                }

                glVertex3f(x+i*w_scale, y+ table->GetCounterValue(c,i)*h_scale, 0.0f);

                if (table->is_cell_selected(0, i))
                {
                    glLineWidth(1.4f);
                    aLabel = "BIN " + QString(table->GetBinValue(0,i).c_str()) + " -> # " + QString(float2stdstr(table->GetCounterValue(0,i)).c_str());
                    is_aLabalActive = true;
                    aLabel_x = x+i*w_scale;
                    aLabel_y = y+ table->GetCounterValue(c,i)*h_scale;

                    if (table->GetCounterValue(c,i) == max_counter)
                        aLabel_y -= 0.1;

                    glColor3f(r,g,b);
                }

            }
            else
            {


                if (table->is_cell_selected(0, i))
                {
                    glColor3f(1.0f,1.0f,1.0f);
                    glLineWidth(4.0f);
                }

                glVertex3f(x+log(i+1)*w_scale, y+table->GetCounterValue(c,i)*h_scale, 0.0f);

                if (table->is_cell_selected(0, i))
                {
                    glLineWidth(1.4f);
                    aLabel = "BIN " + QString(table->GetBinValue(0,i).c_str()) + " -> # " + QString(float2stdstr(table->GetCounterValue(0,i)).c_str());
                    is_aLabalActive = true;
                    aLabel_x = x+i*w_scale;
                    aLabel_y = y+ table->GetCounterValue(c,i)*h_scale;
                    glColor3f(r,g,b);
                }
            }
        }
        glEnd();
        renderBitmapString(float2stdstr(max_counter), x_init, y+h);
    }


    if (is_aLabalActive)
        renderBitmapString(aLabel.toStdString(), aLabel_x, aLabel_y+0.05);



    for (int i=0; i<n_bins; i = i+5)
    {
        if (log_hist == false)
        {
            glBegin(GL_LINES);
            glVertex3f(x+i*w_scale, y+0.005*h, 0.0f);
            glVertex3f(x+i*w_scale, y-0.005*h, 0.0f);
            glEnd();
        }
        else
        {
            glBegin(GL_LINES);
            glVertex3f(x+log(i+1)*w_scale, y+0.005*h, 0.0f);
            glVertex3f(x+log(i+1)*w_scale, y-0.005*h, 0.0f);
            glEnd();
        }
    }
    glEnd();



    int pixel_Xaxeslabel_w = glutBitmapLength(GLUT_FONT, (const unsigned char*)int2stdstr(n_bins).c_str());
    qt2gl_coord(this->width, this->height, GL_H, GL_W, pixel_Xaxeslabel_w, 0, &gl_x, &gl_y);

    glLineWidth(0.75f);
    glLineStipple(1, 0x3F07);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINE_STRIP);
    glVertex3f(x+w,y, 0.0f);
    glVertex3f(x+w-0.5*(gl_x-(-GL_W/2)),y+0.5*h, 0.0f);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    renderBitmapString(int2stdstr(n_bins), -1*(gl_x-(-GL_W/2))+x+w, y+0.5*h);

    glLineWidth(1.0f);
    // draw the axes
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x_init, y, 0.0f);
    glVertex3f(x+w, y, 0.0f);
    glEnd();


    table->setPositionOnScreen_1st_graph(x,y,w,h);



}





int HistogramViewer::get_n_active_features() {

    int n_active = 0;
    n_active += (table_tcp_sip.isActive()==true? 1 : 0);
    n_active += (table_tcp_sport.isActive()==true? 1 : 0);
    n_active += (table_tcp_dip.isActive()==true? 1 : 0);
    n_active += (table_tcp_dport.isActive()==true? 1 : 0);

    n_active += (table_udp_sip.isActive()==true? 1 : 0);
    n_active += (table_udp_sport.isActive()==true? 1 : 0);
    n_active += (table_udp_dip.isActive()==true? 1 : 0);
    n_active += (table_udp_dport.isActive()==true? 1 : 0);

    n_active += (table_icmp_sip.isActive()==true? 1 : 0);
    n_active += (table_icmp_dip.isActive()==true? 1 : 0);


    return n_active;

}


int HistogramViewer::get_max_bincount_features()
{
    int max_bincount = -1;


    max_bincount = max(max_bincount, (table_tcp_sip.isActive()==true? table_tcp_sip.GetNumCols() : -1));
    max_bincount = max(max_bincount, (table_tcp_sport.isActive()==true? table_tcp_sport.GetNumCols() : -1));
    max_bincount = max(max_bincount, (table_tcp_dip.isActive()==true? table_tcp_dip.GetNumCols() : -1));
    max_bincount = max(max_bincount, (table_tcp_dport.isActive()==true? table_tcp_dport.GetNumCols() : -1));

    max_bincount = max(max_bincount, (table_udp_sip.isActive()==true? table_udp_sip.GetNumCols() : -1));
    max_bincount = max(max_bincount, (table_udp_sport.isActive()==true? table_udp_sport.GetNumCols() : -1));
    max_bincount = max(max_bincount, (table_udp_dip.isActive()==true? table_udp_dip.GetNumCols() : -1));
    max_bincount = max(max_bincount, (table_udp_dport.isActive()==true? table_udp_dport.GetNumCols() : -1));

    max_bincount = max(max_bincount, (table_icmp_sip.isActive()==true? table_icmp_sip.GetNumCols() : -1));
    max_bincount = max(max_bincount, (table_icmp_dip.isActive()==true? table_icmp_dip.GetNumCols() : -1));

    return max_bincount;
}

float HistogramViewer::get_max_height_features()
{

    float max_feat = -1;


    max_feat = max(max_feat, (table_tcp_sip.isActive()==true? table_tcp_sip.GetMAXCounter() : -1));
    max_feat = max(max_feat, (table_tcp_sport.isActive()==true? table_tcp_sport.GetMAXCounter() : -1));
    max_feat = max(max_feat, (table_tcp_dip.isActive()==true? table_tcp_dip.GetMAXCounter() : -1));
    max_feat = max(max_feat, (table_tcp_dport.isActive()==true? table_tcp_dport.GetMAXCounter() : -1));

    max_feat = max(max_feat, (table_udp_sip.isActive()==true? table_udp_sip.GetMAXCounter() : -1));
    max_feat = max(max_feat, (table_udp_sport.isActive()==true? table_udp_sport.GetMAXCounter() : -1));
    max_feat = max(max_feat, (table_udp_dip.isActive()==true? table_udp_dip.GetMAXCounter() : -1));
    max_feat = max(max_feat, (table_udp_dport.isActive()==true? table_udp_dport.GetMAXCounter() : -1));

    max_feat = max(max_feat, (table_icmp_sip.isActive()==true? table_icmp_sip.GetMAXCounter() : -1));
    max_feat = max(max_feat, (table_icmp_dip.isActive()==true? table_icmp_dip.GetMAXCounter() : -1));

    return max_feat;

}



int HistogramViewer::paintGL_print_features_active(RowsTable * table, int current_hist, float x, float y, float h, float w, float r, float g, float b)
{
    if (table->isActive()==false)
        return 0;


    table->setPositionOnScreen(x,y,w,h);



    if (table->GetNumRows()>1)
    {
        paintGL_hist_vector_matrix(table, x, y, h, w, r, g, b);
        glColor3f(r,g,b);
        renderBitmapString(table->name.c_str(), x+(0.7*w), y+(0.9*h));
        glColor3f(1,1,1);

    }
    else
    {
        glColor3f(r,g,b);
        renderBitmapString(table->name.c_str(), x+(0.9*w), y+(0.9*h));
        glColor3f(1,1,1);


        paintGL_hist_vector(table, x, y, h, w/2, r, g, b);

        if(table->isIP==true)
            paintGL_Hilbert_map_ip(table, x+w/2, y, h, w/2);
        else if(table->isPort == true)
            paintGL_sorted_port_hist(table, x+w/2, y, h, w/2, r, g, b);
    }
    current_hist++;

    return 1;

}


void HistogramViewer::paintGL_one_whole_histogram(RowsTable * table, float x, float y, float h, float w, float max_w, float max_h, float r, float g, float b)
{
    if (table->isActive()==false)
        return;




    x += 0.01*w;
    y += 0.01*h;

    h -= 0.02*h;
    w -= 0.02*w;






    table->setPositionOnScreen(x,y,w,h);

    QString aLabel;
    float aLabel_x, aLabel_y;
    bool is_aLabalActive = false;

    glColor3f(r,g,b);
    glLineWidth(1.4f);


    float w_scale = (log_hist == false? (float) w/max_w : (float) w/log(max_w));
    float h_scale = h/max_h;


    int n_bins = table->GetNumCols();


    qWarning() << n_bins;

    glBegin(GL_LINE_STRIP);


    for (int i=0; i<n_bins; i++)
    {
        if (log_hist == false)
        {
            glVertex3f(x+i*w_scale, y+ table->GetCounterValue(0,i)*h_scale, 0.0f);

            //qWarning() << x+i*w_scale <<  y+ table->GetCounterValue(0,i)*h_scale;
        }
        else
        {
            glVertex3f(x+log(i+1)*w_scale, y+table->GetCounterValue(0,i)*h_scale, 0.0f);

            //qWarning() << x+log(i+1)*w_scale << y+table->GetCounterValue(0,i)*h_scale;
        }
    }


    if (log_hist == false)
        glVertex3f(x+(n_bins-1)*w_scale, y, 0.0f);

    else
        glVertex3f(x+log(n_bins)*w_scale, y, 0.0f);

    glEnd();


    glPointSize(5.0f);
    glBegin(GL_POINTS);
    if (log_hist == false)
        glVertex3f(x+(n_bins-1)*w_scale, y, 0.0f);
    else
        glVertex3f(x+log(n_bins)*w_scale, y, 0.0f);
    glEnd();
    glPointSize(1.0f);




}


void HistogramViewer::paintGL_one_whole_histogram_default()
{


    float n_bins = (float) this->get_max_bincount_features();
    float max_counter = this->get_max_height_features();

    float w_scale = (log_hist == false? (float) GL_W/max_counter : (float) GL_W/log(max_counter));

    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    float x = -GL_W/2 + 0.01*GL_W;
    float y = -GL_H/2 + 0.01*GL_H;

    float h = GL_H - 0.02*GL_H;
    float w = GL_W - 0.02*GL_W;
    renderBitmapString(float2stdstr(max_counter), x, y+h);
    renderBitmapString(float2stdstr(n_bins), x+0.995*w, y+0.005*h);
    glBegin(GL_LINES);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x+w, y, 0.0f);
    glEnd();
    for (int i=0; i<n_bins; i = i+5)
    {
        if (log_hist == false)
        {
            glBegin(GL_LINES);
            glVertex3f(x+i*w_scale, y+0.005*h, 0.0f);
            glVertex3f(x+i*w_scale, y-0.005*h, 0.0f);
            glEnd();
        }
        else
        {
            glBegin(GL_LINES);
            glVertex3f(x+log(i+1)*w_scale, y+0.005*h, 0.0f);
            glVertex3f(x+log(i+1)*w_scale, y-0.005*h, 0.0f);
            glEnd();
        }
    }


    this->paintGL_one_whole_histogram(&table_icmp_sip, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_icmp_sip);
    this->paintGL_one_whole_histogram(&table_icmp_dip, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_icmp_dip);

    this->paintGL_one_whole_histogram(&table_udp_dport, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_udp_dport);
    this->paintGL_one_whole_histogram(&table_udp_dip, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_udp_dip);
    this->paintGL_one_whole_histogram(&table_udp_sport, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_udp_sport);
    this->paintGL_one_whole_histogram(&table_udp_sip, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_udp_sip);

    this->paintGL_one_whole_histogram(&table_tcp_dport, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_tcp_dport);
    this->paintGL_one_whole_histogram(&table_tcp_dip, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_tcp_dip);
    this->paintGL_one_whole_histogram(&table_tcp_sport, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_tcp_sport);
    this->paintGL_one_whole_histogram(&table_tcp_sip, -GL_W/2, -GL_H/2, GL_H, GL_W, n_bins, max_counter, COLOR_H_tcp_sip);




}


void HistogramViewer::paintGL()
{



    glMatrixMode(GL_MODELVIEW);

    if (this->is_selection_mode_opengl)
        this->startPicking(click.x(), click.y());


    std::cout << "[BEGIN] PAINT " << std::endl;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int n_hist = this->get_n_active_features();

    std::cout << n_hist << std::endl;


    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    //this->renderBitmapString("Press SPACE for the all-in-one histogram visualization", 3.0, 4.90);

    if (this->one_global_histogram == false)
    {


        int current_hist = 0;

        current_hist += paintGL_print_features_active(&table_icmp_sip, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_icmp_sip);
        current_hist += paintGL_print_features_active(&table_icmp_dip, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_icmp_dip);

        current_hist += paintGL_print_features_active(&table_udp_dport, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_udp_dport);
        current_hist += paintGL_print_features_active(&table_udp_dip, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_udp_dip);
        current_hist += paintGL_print_features_active(&table_udp_sport, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_udp_sport);
        current_hist += paintGL_print_features_active(&table_udp_sip, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_udp_sip);

        current_hist += paintGL_print_features_active(&table_tcp_dport, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_tcp_dport);
        current_hist += paintGL_print_features_active(&table_tcp_dip, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_tcp_dip);
        current_hist += paintGL_print_features_active(&table_tcp_sport, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_tcp_sport);
        current_hist += paintGL_print_features_active(&table_tcp_sip, current_hist, -GL_W/2,-GL_H/2+GL_H*(float)current_hist/(float)n_hist,GL_H/n_hist,GL_W, COLOR_H_tcp_sip);
    }
    else
    {
        paintGL_one_whole_histogram_default();
    }




    glFlush();


    if (this->is_selection_mode_opengl)
        this->stopPicking();

        glutSwapBuffers();
    std::cout << "[END] PAINT " << std::endl;
}


void HistogramViewer::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->pos();
    float x,y;

    qt2gl_coord(this->width, this->height, GL_H, GL_W, p.x(), p.y(), &x, &y);

    RowsTable * target_table = NULL;

    if (table_tcp_sip.pointIsInside(x,y)) target_table = &table_tcp_sip;
    if (table_tcp_sport.pointIsInside(x,y)) target_table = &table_tcp_sport;
    if (table_tcp_dip.pointIsInside(x,y)) target_table = &table_tcp_dip;
    if (table_tcp_dport.pointIsInside(x,y)) target_table = &table_tcp_dport;

    if (table_udp_sip.pointIsInside(x,y)) target_table = &table_udp_sip;
    if (table_udp_sport.pointIsInside(x,y)) target_table = &table_udp_sport;
    if (table_udp_dip.pointIsInside(x,y)) target_table = &table_udp_dip;
    if (table_udp_dport.pointIsInside(x,y)) target_table = &table_udp_dport;

    if (table_icmp_sip.pointIsInside(x,y)) target_table = &table_icmp_sip;
    if (table_icmp_dip.pointIsInside(x,y)) target_table = &table_icmp_dip;


    if (target_table != NULL)
    {
        if (target_table->isPort && target_table->GetNumRows()==1)
            this->mouse_over_sorted_port_hist(target_table, x, y);

        if (target_table->GetNumRows()==1)
            this->mouse_over_unsorted_hist(target_table, x, y);

        std::cout << target_table->GetNumRows() << std::endl;
        std::cout << (event->type() == QEvent::MouseButtonPress? "true" : "false") << std::endl;

        if ((target_table->GetNumRows()>1) && (event->type() == QEvent::MouseButtonPress)  )
        {
            this->click.setX(event->x());
            this->click.setY(event->y());
            this->is_selection_mode_opengl = true;
        }
    }

    repaint();


}



void HistogramViewer::mousePressEvent(QMouseEvent *event)
{

    this->mouseMoveEvent(event);
}




void HistogramViewer::startPicking(int x, int y)
{

    std::cout << "Start picking" << std::endl;
    GLint viewport[4];
    float ratio;
    glSelectBuffer(bufsize, selectBuf);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glGetIntegerv(GL_VIEWPORT, viewport);

    glRenderMode(GL_SELECT);
    glInitNames();


    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, viewport[3]-y, 5, 5, viewport);
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

 }



void HistogramViewer::stopPicking()
{
        std::cout << "Stop picking" << std::endl;
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glFlush();
        GLint hits = glRenderMode(GL_RENDER);
        if (hits != 0){
                processHits2(hits,selectBuf,0);
                std::cout << "Stop picking processHit" << std::endl;
        }
        else{
            this->matrix_timestamp_clicked.clear();
            this->timestamp_selected_V.clear();
            emit selected_timestamps(timestamp_selected_V);
        }



        this->is_selection_mode_opengl = false;

        this->resizeGL(width, height);
}


void HistogramViewer::processHits2 (GLint hits, GLuint buffer[], int sw)
{

   GLint i, j, numberOfNames=0;
   GLuint names, *ptr=NULL, minZ,*ptrNames=NULL;

   ptr = (GLuint *) buffer;
   minZ = 0xffffffff;
   for (i = 0; i < hits; i++) {
      names = *ptr;
          ptr++;
          if (*ptr < minZ) {
                  numberOfNames = names;
                  minZ = *ptr;
                  ptrNames = ptr+2;
          }

          ptr += names+2;
        }
   if (numberOfNames > 0) {
          printf ("You picked object  ");
          ptr = ptrNames;
          for (j = 0; j < numberOfNames; j++,ptr++) {
                 printf ("PICKING!!!!  %d\n", *ptr);
                 this->matrix_timestamp_clicked.push_back( (int) (*ptr));



                 this->timestamp_selected_V.push_back(timestamp_V[((int) (*ptr)) + timestamp_init_index]);
                 emit selected_timestamps(timestamp_selected_V);
                }
        }
   else
   {
           printf("You click a object without name!");
           this->matrix_timestamp_clicked.clear();
           this->timestamp_selected_V.clear();
           emit selected_timestamps(timestamp_selected_V);
       }

   printf ("\n");


   return;

}

