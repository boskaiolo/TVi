#include "matrix_visualizer.h"



#include <QDebug>
#include <QProgressDialog>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <GLUT/glut.h>


#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/module/HierarchyLayoutModule.h>
#include <ogdf/module/RankingModule.h>
#include <ogdf/module/HierarchyLayoutModule.h>
#include <ogdf/layered/FastHierarchyLayout.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>


#include "pixel2gl.h"
#include "db_credential.h"
#include "tools.h"


#define GLUT_FONT GLUT_BITMAP_8_BY_13
#define GLUT_FONT_HEIGHT 14



MatrixVisualizer::MatrixVisualizer(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    db = QSqlDatabase::addDatabase("QMYSQL", "matrix_mysql_connection");
    db.setHostName(DB_host);
    db.setDatabaseName(DB_name);
    db.setUserName(DB_user);
    db.setPassword(DB_pass);

    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";

    is_selection_mode_opengl = false;
    this->setMouseTracking(true);

    selectBuf = (GLuint*) malloc(sizeof(GLuint) * 2000000);
    bufsize = sizeof(GLuint) * 2000000;
    this->selected_obj = -1;

    this->timer  = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint_slot()));
    timer->setInterval(50);


}


MatrixVisualizer::~MatrixVisualizer()
{
    this->clean_who_vectors();
    db.close();
}


void MatrixVisualizer::initializeGL()
{
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

    glTranslatef(0.0f, 0.0f, -GL_H/2);


    if (GL_H != GL_W) {
        qFatal("[matrixvisualizer] look at the define: GL_H and GL_W MUST BE EQUAL!!!!");
    }

 }



void MatrixVisualizer::mouseMoveEvent(QMouseEvent *event)
{
    this->click.setX(event->x());
    this->click.setY(event->y());
    this->is_selection_mode_opengl = true;

    timer->start();
    repaint();

}



void MatrixVisualizer::resizeGL(int width, int height)
 {
    this->width = width;
    this->height = height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
 }



int MatrixVisualizer::maxheight()
{

    int temp_max = -1;

    for (int i=0; i< this->link_height.size(); i++)
    {
        if(link_height[i]>temp_max)
            temp_max = link_height[i];
    }

    return temp_max;
}

void MatrixVisualizer::search_indexes(uint32_t from, uint32_t to, int & row, int & col)
{
    for (int i=0; i< ranking_1.size(); i++)
        if (ranking_1[i] == from)
        {
            row = i;
            break;
        }


    for (int i=0; i< ranking_2.size(); i++)
        if (ranking_2[i] == to)
        {
            col = i;
            break;
        }

    return;

}


void MatrixVisualizer::paintGL_matrix()
{


    int n_rows = this->ranking_1.size();
    int n_cols = this->ranking_2.size();

    double d_x = (0.8*GL_W) / (double)n_cols;
    double d_y = (0.8*GL_H) / (double)n_rows;

    double off_x = -0.4*GL_W;
    double off_y = -0.4*GL_H;

    double w_x = (0.8*GL_W);
    double w_y = (0.8*GL_H);


    float min_val = 0.0f;


    unsigned char rgb[3];
    glColor3f(1.0f, 1.0f, 1.0f);

    glPointSize(4.0f);



    // grid
    glColor3f(0.4f, 0.4f, 0.4f);
    glLineWidth(0.5f);

    if (this->is_selection_mode_opengl == false)
    {
        for (int i=0; i<n_rows+1; i++)
        {
            glBegin(GL_LINES);
            glVertex3f(off_x,  off_y+d_y*i, 0.0f);
            glVertex3f(off_x + w_x, off_y+d_y*i, 0.0f);
            glEnd( );
        }
        for(int j=0; j<n_cols+1; j++)
        {
            glBegin(GL_LINES);
            glVertex3f(off_x+j*d_x, off_y, 0.0f);
            glVertex3f(off_x+j*d_x, off_y+w_y, 0.0f);
            glEnd( );
        }
    }

    /*if (this->is_selection_mode_opengl == false)
    {
        for (int i=0; i<n_rows; i++)
        {
            for(int j=0; j<n_cols; j++)
            {
                glBegin(GL_LINE_LOOP);
                glVertex3f(off_x + d_x*j, off_y+d_y*i, 0.0f);
                glVertex3f(off_x + d_x*(j+1), off_y+d_y*i, 0.0f);
                glVertex3f(off_x + d_x*(j+1), off_y+d_y*(i+1), 0.0f);
                glVertex3f(off_x + d_x*j, off_y+d_y*(i+1), 0.0f);
                glEnd( );

            }
        }
    }*/
    glLineWidth(1.0f);


    for (int i=0; i< who_1.size(); i++)
    {
        glPushMatrix();
        glPushName(i);

        int r = pos_r[i];
        int c = pos_c[i];

        float value = (float) this->link_height[i];
        //jetColorMap(rgb, value, min_val, max_height);
        hotColorMap(rgb, value, min_val, max_height);

        float rf = ((float)rgb[0]) / 255.0;
        float gf = ((float)rgb[1]) / 255.0;
        float bf = ((float)rgb[2]) / 255.0;


        float alpha = (this->selected_obj == -1 || this->selected_obj == i) ? 1.0f : 0.7f;



        glColor4f(rf,gf,bf,alpha);

        glBegin(GL_QUADS);
        glVertex3f(off_x + d_x*c, off_y+d_y*r, 0.0f);
        glVertex3f(off_x + d_x*(c+1), off_y+d_y*r, 0.0f);
        glVertex3f(off_x + d_x*(c+1), off_y+d_y*(r+1), 0.0f);
        glVertex3f(off_x + d_x*c, off_y+d_y*(r+1), 0.0f);
        glEnd( );


        glPopName();
        glPopMatrix();

    }





    //legend
    for (int i=0; i<=100; i++)
    {

        float val = min_val + ( (max_height - min_val) / 100.0 * (float)i);

        //jetColorMap(rgb, val, min_val, max_height);
        hotColorMap(rgb, val, min_val, max_height);

        float rf = ((float)rgb[0]) / 255.0;
        float gf = ((float)rgb[1]) / 255.0;
        float bf = ((float)rgb[2]) / 255.0;

        glColor3f(rf,gf,bf);

        glBegin(GL_QUADS);
        glVertex3f(2+2.0*i/100.0, -5, 0.0f);
        glVertex3f(2+2.0*i/100.0, -4.5, 0.0f);
        glVertex3f(2+2.0*(i+1)/100.0, -4.5, 0.0f);
        glVertex3f(2+2.0*(i+1)/100.0, -5, 0.0f);
        glEnd( );
    }


    if (this->selected_obj != -1)
    {
        int i = this->selected_obj;

        int r = pos_r[i];
        int c = pos_c[i];


        glColor4f(1.0f, 1.0f, 1.0f, 0.8);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        glVertex3f(off_x, off_y+d_y*(r+0.5), 0.0f);
        glVertex3f(off_x + 0.8*GL_W, off_y+d_y*(r+0.5), 0.0f);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(off_x + d_x*(c+0.5), off_y, 0.0f);
        glVertex3f(off_x + d_x*(c+0.5), off_y+0.8*GL_H, 0.0f);
        glEnd();


        this->renderBitmapString((axes_y_name == "sport" || axes_y_name == "dport") ?  std::string(uint2str(who_1[selected_obj])) : ip_uint32_to_stdstring(who_1[selected_obj]), 4.0, off_y+d_y*(r+0.5));
        this->renderBitmapString((axes_x_name == "sport" || axes_x_name == "dport") ?  std::string(uint2str(who_2[selected_obj])) : ip_uint32_to_stdstring(who_2[selected_obj]), off_x + d_x*(c+0.5), 4.0);



        float value = (float) this->link_height[i];

        int percent = round(100.0*((value-min_val)/(max_height-min_val)));

        glLineStipple(1, 0x3F07);
        glEnable(GL_LINE_STIPPLE);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex3f(2+2.0*percent/100.0, -5, 0.0f);
        glVertex3f(2+2.0*percent/100.0, -4.4, 0.0f);
        glEnd( );
        glLineWidth(1.0f);
        glDisable(GL_LINE_STIPPLE);

        glColor3f(1.0f, 1.0f, 1.0f);
        this->renderBitmapString(float2stdstr((float) this->link_height[i]), 2.05+2.0*percent/100.0, -4.86);

    }


    this->renderBitmapString(float2stdstr(min_val), 2, -4.5);
    this->renderBitmapString(float2stdstr(max_height), 4, -4.5);



    glPointSize(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);

}


void MatrixVisualizer::renderBitmapString(std::string txt, float x, float y)
{
    glRasterPos3f(x, y,0);
    for (size_t c = 0; c<txt.length(); c++) {
      glutBitmapCharacter(GLUT_FONT, txt[c]);
    }
}


void MatrixVisualizer::writeLabels()
{

    this->renderBitmapString(this->axes_x_name, 0.0f, -4.85f);
    this->renderBitmapString(this->axes_y_name, -4.85f, 0.0f);

    if (this->selected_obj != -1)
    {
        std::string target1 = (axes_y_name == "sport" || axes_y_name == "dport") ?  std::string(uint2str(who_1[selected_obj])) : ip_uint32_to_stdstring(who_1[selected_obj]);
        std::string target2 = (axes_x_name == "sport" || axes_x_name == "dport") ?  std::string(uint2str(who_2[selected_obj])) : ip_uint32_to_stdstring(who_2[selected_obj]);

        this->renderBitmapString(target1 + " -> " + target2 + " #=" + int2str(link_height[selected_obj]), -1.0f, 4.85f);
    }


}


void MatrixVisualizer::paintGL()
{
    glMatrixMode(GL_MODELVIEW);


    if (this->is_selection_mode_opengl)
        this->startPicking(click.x(), click.y());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    paintGL_matrix();
    writeLabels();



    if (this->is_selection_mode_opengl)
        this->stopPicking();


    glFlush();

    glutSwapBuffers();
}



void MatrixVisualizer::draw_matrix_on_signal(int proto, int feat_x, int feat_y, int feat_height, int port_filter_begin, int port_filter_end)
{
    this->fetch_data(proto, feat_x, feat_y, feat_height, port_filter_begin, port_filter_end);
    this->max_height = this->maxheight();
    this->build_matrix();
    repaint();
}




void MatrixVisualizer::clean_who_vectors()
{
    who_1.clear();
    who_2.clear();
    link_height.clear();

    this->pos_c.clear();
    this->pos_r.clear();
}


void MatrixVisualizer::time_clicked(int timestamp_init, int timestamp_end)
{

    this->timestamp_init = timestamp_init;
    this->timestamp_end = timestamp_end;
    if (timestamp_init == timestamp_end)
        this->timestamp_end += DB_DELTA_HIST;

    repaint();
}


void MatrixVisualizer::fetch_data(int proto, int feat_x, int feat_y, int feat_height, int port_filter_begin, int port_filter_end)
{

    if (this->isHidden() == true)
        return;


    this->clean_who_vectors();

    QProgressDialog * progress = new QProgressDialog(this);
    progress->setLabelText("Fetching data from MySql DB");
    progress->setRange(0, 2);
    progress->setMinimumDuration (1);
    progress->setValue(1);
    progress->setModal(true);

    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";
    QSqlQuery q;


    QString select_1 = QString((feat_x == 0 ? "sip" : (feat_x == 1 ? "sport" : (feat_x == 2 ? "dip" : "dport")) ));
    QString select_2 = QString((feat_y == 0 ? "sip" : (feat_y == 1 ? "sport" : (feat_y == 2 ? "dip" : "dport")) ));
    QString select_3 = QString((feat_height == -1 ? "1" : (feat_height == 0 ? "count(0)" : (feat_height == 1 ? "sum(dip2sip_pk)+sum(sip2dip_pk)" : "sum(dip2sip_byte)+sum(sip2dip_byte)")) ));


    QString from_table = QString(DB_PCAP_TABLE);
    QString protocol = QString(int2str(proto));

    QString port_inf = QString(int2str(proto==1? -1: port_filter_begin ));
    QString port_sup = QString(int2str(proto==1? 65536: port_filter_end ));


    QString timestamp_inf = QString(int2str(timestamp_init));
    QString timestamp_sup = QString(int2str(timestamp_end));

    QString comma = QString(", ");

    QString strquery;

    strquery  = QString("SELECT ") + select_1 + comma + select_2 + comma + select_3 + \
                QString(" FROM ") + from_table + \
                QString(" WHERE proto = ") + protocol +\
                QString(" AND (( sport>=") + port_inf + QString(" AND sport<= ") + port_sup +\
                QString (") OR ( dport>=") + port_inf + QString(" AND dport<= ") + port_sup +\
                QString(")) AND timestamp >= ") + timestamp_inf + QString(" AND timestamp <= ") + timestamp_sup +\
                QString(" GROUP BY ") + select_1 + comma + select_2 + QString(";");

    q = db.exec(strquery);

    if (q.size() > 0)
    {
        q.seek(-1);
        while ( q.next() )
        {
            if (feat_x == 0 || feat_x == 2)
                who_1.push_back(ip_stdstring_to_uint32(q.value(0).toString().toStdString()));
            else
                who_1.push_back(q.value(0).toUInt());

            if (feat_y == 0 || feat_y == 2)
                who_2.push_back(ip_stdstring_to_uint32(q.value(1).toString().toStdString()));
            else
                who_2.push_back(q.value(1).toUInt());

            link_height.push_back(q.value(2).toInt());
        }
    }
    else
    {
        qWarning() <<  "Empty table with query " << strquery;
    }

    progress->~QProgressDialog();

    std::cout << strquery.toStdString() << std::endl;



    this->axes_x_name = std::string((feat_y == 0 ? "sip" : (feat_y == 1 ? "sport" : (feat_y == 2 ? "dip" : "dport")) ));
    this->axes_y_name = std::string((feat_x == 0 ? "sip" : (feat_x == 1 ? "sport" : (feat_x == 2 ? "dip" : "dport")) ));


}




int MatrixVisualizer::value_to_index(uint32_t value, std::vector<uint32_t> stringvector)
{
    for (size_t i=0; i<stringvector.size(); i++)
    {
        if ( fabs(value - stringvector[i]) < 0.001)
            return i;
    }
    return -1;
}



void MatrixVisualizer::startPicking(int x, int y)
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
    gluPickMatrix(x, viewport[3]-y, 1, 1, viewport);
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
 }



void MatrixVisualizer::stopPicking()
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
    else
    {
        selected_obj = -1;
    }



    this->is_selection_mode_opengl = false;

    this->resizeGL(width, height);

}


void MatrixVisualizer::processHits2 (GLint hits, GLuint buffer[], int sw)
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
          selected_obj = INT_MAX;
          for (j = 0; j < numberOfNames; j++,ptr++) {
                 printf ("PICKING!!!!  %d\n", *ptr);
                 if (selected_obj > (int) *ptr )
                     selected_obj = (int) *ptr;
                }
        }
   else
   {
           printf("You click a object without name!");
           selected_obj = -1;
       }

   printf ("\n");


   return;

}






void MatrixVisualizer::build_matrix()
{

    using namespace ogdf;


    if (this->isHidden() == true)
        return;
    if (this->who_1.size() == 0)
        return;




    QProgressDialog * progress = new QProgressDialog(this);

    progress->setLabelText("Building SugiyamaLayout");
    progress->setRange(0, 10);
    progress->setMinimumDuration (1);
    progress->setValue(5);
    progress->setModal(true);






    this->ranking_1.clear();
    this->ranking_2.clear();
    this->node1_name_vector.clear();
    this->node2_name_vector.clear();



    Graph G;
    GraphAttributes GA(G);

    std::vector<ogdf::node> node1_obj_list;
    std::vector<ogdf::node> node2_obj_list;


    std::list<uint32_t> node1_name_temp_list;
    std::list<uint32_t> node2_name_temp_list;


    for (size_t i=0; i<this->who_1.size(); i++)
    {
        node1_name_temp_list.push_back(who_1[i]);
        node2_name_temp_list.push_back(who_2[i]);
    }
    node1_name_temp_list.sort();
    node1_name_temp_list.unique();

    node2_name_temp_list.sort();
    node2_name_temp_list.unique();


    //progress->setValue(2);
    qWarning() << "VAL 2";


    // Add nodes
    for ( std::list<uint32_t>::iterator it = node1_name_temp_list.begin() ; it != node1_name_temp_list.end(); it++ )
    {
        node1_obj_list.push_back(G.newNode());
        node1_name_vector.push_back(*it);
    }
    for ( std::list<uint32_t>::iterator it = node2_name_temp_list.begin() ; it != node2_name_temp_list.end(); it++ )
    {
        node2_obj_list.push_back(G.newNode());
        node2_name_vector.push_back(*it);
    }



    //progress->setValue(3);
    qWarning() << "VAL 3";


    // Add links
    ogdf::node * w1;
    ogdf::node * w2;

    for (size_t i=0; i<this->who_1.size(); i++)
    {
        int j=0;
        for ( std::list<uint32_t>::iterator it = node1_name_temp_list.begin() ; it != node1_name_temp_list.end(); it++,j++ )
        {
            if (who_1[i] == *it)
            {
                w1 = &node1_obj_list[j];
                break;
            }
        }

        j=0;

        for ( std::list<uint32_t>::iterator it = node2_name_temp_list.begin() ; it != node2_name_temp_list.end(); it++,j++ )
        {
            if (who_2[i] == *it)
            {
                w2 = &node2_obj_list[j];
                break;
            }
        }

        G.newEdge(*w1, *w2);
    }



    GA.writeGML("/Users/bosko/Desktop/pre_layout.gml");


    //progress->setValue(4);
    qWarning() << "VAL 4";

    SugiyamaLayout SL;

    //SL.setRanking(new OptimalRanking);
    //SL.setCrossMin(new MedianHeuristic);
    //SL.setLayout(new FastHierarchyLayout);


    qWarning() << "SL.call(GA)_pre";
    SL.call(GA);
    qWarning() << "SL.call(GA)_post";

    qWarning() << "VAL 5";

    GA.writeGML("/Users/bosko/Desktop/post_layout.gml");




    //progress->setValue(6);
    qWarning() << "VAL 6";






    std::vector<double> x_node;
    for (int i=0; i<node1_name_vector.size(); i++)
    {
        x_node.push_back(GA.x(node1_obj_list[i]));
    }


    for (int i=0; i<node1_name_vector.size(); i++)
    {
        double min_x = DBL_MAX-10;
        int index = 0;


        for (int j=0; j<node1_name_vector.size(); j++)
        {
            if(x_node[j]<min_x)
            {
                min_x =x_node[j];
                index = j;
            }
        }


        this->ranking_1.push_back(node1_name_vector[index]);
        x_node[index] = DBL_MAX;

    }

    //progress->setValue(7);
    qWarning() << "VAL 7";


    x_node.clear();
    for (int i=0; i<node2_name_vector.size(); i++)
    {
        x_node.push_back(GA.x(node2_obj_list[i]));
    }


    for (int i=0; i<node2_name_vector.size(); i++)
    {
        double min_x = DBL_MAX-10;
        int index = 0;


        for (int j=0; j<node2_name_vector.size(); j++)
        {
            if(x_node[j]<min_x)
            {
                min_x =x_node[j];
                index = j;
            }
        }


        this->ranking_2.push_back(node2_name_vector[index]);
        x_node[index] = DBL_MAX;

    }

    //progress->setValue(8);
    qWarning() << "VAL 8";


    for (int i=0; i< who_1.size(); i++)
    {
        int r,c;
        search_indexes(who_1[i], who_2[i], r, c);
        this->pos_r.push_back(r);
        this->pos_c.push_back(c);
    }

    //progress->setValue(10);
    qWarning() << "VAL 10";




    G.clear();
    node1_obj_list.clear();
    node2_obj_list.clear();
    node1_name_temp_list.clear();
    node2_name_temp_list.clear();


    delete progress;

}
