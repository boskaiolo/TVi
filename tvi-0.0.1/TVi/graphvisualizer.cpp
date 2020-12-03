#include "graphvisualizer.h"

#include <QDebug>
#include <QProgressDialog>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <QtConcurrentRun>
#include <GLUT/glut.h>
#include <list>
#include <algorithm>
#include <arpa/inet.h>

#include <ogdf/energybased/FMMMLayout.h>

#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/module/HierarchyLayoutModule.h>




#include "pixel2gl.h"
#include "db_credential.h"
#include "tools.h"


#define GLUT_FONT GLUT_BITMAP_8_BY_13
#define GLUT_FONT_HEIGHT 14


using namespace ogdf;




void graphVisualizer::keyPressEvent(QKeyEvent *event)
{
    int keypressed = event->key();

    if (keypressed == Qt::Key_Space)
    {
        this->graph_over_map =!this->graph_over_map;
        selected_obj = -1;

        qWarning() << "graph_over_map " << this->graph_over_map;
        repaint();
    }

    else if (keypressed == Qt::Key_B)
    {
        this->graph_with_bg =!this->graph_with_bg;

        qWarning() << "graph_over_no_map " << this->graph_over_map;
        repaint();
    }

    else if (keypressed == Qt::Key_H)
    {
        this->link_hidden =!this->link_hidden;

        qWarning() << "link_hidden " << this->link_hidden;
        repaint();
    }
}




void graphVisualizer::computeLayout_worldmap()
{


    QProgressDialog * progress = new QProgressDialog(this);
    progress->setLabelText("computeLayout_worldmap()");
    progress->setRange(0, node_name_list.size());
    progress->setMinimumDuration (1);
    progress->setValue(1);
    progress->setModal(true);




    qWarning() << "computeLayout_worldmap() BEGIN";

    world_node_pos_x.clear();
    world_node_pos_y.clear();

    db.setDatabaseName(DB_ip2coord);
    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";

    double x_default, y_default;
    toMollweide(GPS_LATITUDE, GPS_LONGITUDE, x_default, y_default);

    for ( size_t i=0; i< node_name_list.size(); i++ )
    {

        progress->setValue(i);


        QString query = QString("SELECT latitude, longitude from ip_group_city where ip_start<=") + QString(uint2str(node_name_list[i])) + QString(" order by ip_start desc limit 1;");

        QSqlQuery q = db.exec(query);
        if (q.size() > 0)
        {
            q.seek(-1);
            while ( q.next() )
            {
                double latitude = q.value(0).toDouble();
                double longitude = q.value(1).toDouble();


                double x,y;

                if (latitude <0.0001 && longitude <0.0001)
                {
                    x=x_default;
                    y=y_default;
                }
                else
                {
                    toMollweide(latitude, longitude, x, y);
                }


                this->world_node_pos_x.push_back(x);
                this->world_node_pos_y.push_back(y);


                //qWarning() << query;
                //qWarning() << ip_uint32_to_stdstring(node_name_list[i]).c_str() << latitude << longitude << x << y;

            }
        }
        else
        {
            qWarning() <<  "Empty table with query " << query;
        }

    }



    for ( size_t i=0; i< node_name_list.size(); i++ )
    {
        double pos_x, pos_y;
        search_world_position_from_name(node_name_list[i], &pos_x, &pos_y);

        bool flag = false;

        for (int a=0; a<IPpositions.size(); a++)
        {
            if (( fabs(pos_x - IPpositions[a].x) + fabs(pos_y - IPpositions[a].y)) < 0.001)
            {
                IPpositions[a].ip.push_back(node_name_list[i]);
                flag = true;
                break;
            }
        }

        if (flag == false)
        {
            worldIPposition wip;
            wip.x = pos_x;
            wip.y = pos_y;
            wip.ip.push_back(node_name_list[i]);
            IPpositions.push_back(wip);
        }
    }


    for ( size_t i=0; i< who_1_orig.size(); i++)
    {
        double pos_x, pos_y;
        search_world_position_from_name(who_1_orig[i], &pos_x, &pos_y);

        for (int a=0; a<IPpositions.size(); a++)
        {
            if (( fabs(pos_x - IPpositions[a].x) + fabs(pos_y - IPpositions[a].y)) < 0.001)
            {
                who_1_world_pos_pointer.push_back(a);
                break;
            }
        }

    }

    for ( size_t i=0; i< who_2_orig.size(); i++)
    {
        double pos_x, pos_y;
        search_world_position_from_name(who_2_orig[i], &pos_x, &pos_y);

        for (int a=0; a<IPpositions.size(); a++)
        {
            if (( fabs(pos_x - IPpositions[a].x) + fabs(pos_y - IPpositions[a].y)) < 0.001)
            {
                who_2_world_pos_pointer.push_back(a);
                break;
            }
        }

    }



    qWarning() << "computeLayout_worldmap() END";

    progress->~QProgressDialog();

}



void graphVisualizer::create_lists()
{
    std::list<uint32_t> node_name_temp_list;

    for (size_t i=0; i<this->who_1_orig.size(); i++)
    {
        node_name_temp_list.push_back(who_1_orig[i]);
        node_name_temp_list.push_back(who_2_orig[i]);
    }
    node_name_temp_list.sort();
    node_name_temp_list.unique();


    node_name_list.clear();

    // Add nodes
    for ( std::list<uint32_t>::iterator it = node_name_temp_list.begin() ; it != node_name_temp_list.end(); it++ )
    {
        this->node_name_list.push_back((*it));
    }


    node_name_temp_list.clear();
}



void graphVisualizer::computeLayout()
{


    QProgressDialog * progress = new QProgressDialog(this);
    progress->setLabelText("FMMMcomputeLayout()");
    progress->setRange(0, 10);
    progress->setMinimumDuration (1);
    progress->setValue(1);
    progress->setModal(true);

    qWarning() << "computeLayout() BEGIN";

    Graph G;
    GraphAttributes GA(G);

    std::vector<ogdf::node> node_obj_list;


    //progress->setValue(2);
    qWarning() << "computeLayout() uniques ip done";


    node_obj_list.clear();
    this->node_pos_x.clear();
    this->node_pos_y.clear();



    // Add nodes
    for ( int i=0; i<node_name_list.size(); i++ )
    {
        node_obj_list.push_back(G.newNode());
    }


    //progress->setValue(3);
    qWarning() << "computeLayout() nodes defined done";

    // Add links
    ogdf::node * w1;
    ogdf::node * w2;

    for (size_t i=0; i<this->who_1_orig.size(); i++)
    {

        for (size_t j = 0; j< node_name_list.size(); j++)
        {
            if (who_1_orig[i] == node_name_list[j])
            {
                w1 = &node_obj_list[j];
                break;
            }
        }

        for (size_t j = 0; j< node_name_list.size(); j++)
        {
            if (who_2_orig[i] == node_name_list[j])
            {
                w2 = &node_obj_list[j];
                break;
            }
        }

        G.newEdge(*w1, *w2);
    }

    //progress->setValue(5);
    qWarning() << "computeLayout() vertexes defined done";

    node v;
    forall_nodes(v,G)
    GA.width(v) = GA.height(v) = 10.0;
    FMMMLayout fmmm;
    fmmm.qualityVersusSpeed(FMMMLayout::qvsNiceAndIncredibleSpeed);
    fmmm.call(GA);

    //progress->setValue(7);
    qWarning() << "computeLayout() FMMMLayout done";


    for (size_t i = 0; i< node_name_list.size(); i++)
    {
        this->node_pos_x.push_back(GA.x(node_obj_list[i]));
        this->node_pos_y.push_back(GA.y(node_obj_list[i]));
    }



    double min=DBL_MAX, max=DBL_MIN;
    for (size_t i = 0; i< node_pos_x.size(); i++)
    {
        if (node_pos_x[i]<min)
            min = node_pos_x[i];
        else if (node_pos_x[i] > max)
            max = node_pos_x[i];

        if (node_pos_y[i]<min)
            min = node_pos_y[i];
        else if (node_pos_y[i] > max)
            max = node_pos_y[i];
    }
    for (size_t i = 0; i< node_pos_x.size(); i++)
    {
        node_pos_x[i] = (node_pos_x[i]-min) / (max-min);
        node_pos_y[i] = (node_pos_y[i]-min) / (max-min);
    }



    //progress->setValue(8);
    qWarning() << "computeLayout() placement normalization done";

    G.clear();




    for (int c=0; c<this->who_1_orig.size(); c++)
    {
        double pos_1_x, pos_1_y, pos_2_x, pos_2_y;
        search_position_from_names(this->who_1_orig[c], this->who_2_orig[c], &pos_1_x, &pos_1_y, &pos_2_x, &pos_2_y);
        this->who_1_orig_pos_x.push_back(pos_1_x);
        this->who_1_orig_pos_y.push_back(pos_1_y);
        this->who_2_orig_pos_x.push_back(pos_2_x);
        this->who_2_orig_pos_y.push_back(pos_2_y);
    }


    progress->setValue(10);
    qWarning() << "computeLayout() links placement done";




    qWarning() << "computeLayout() END";

    progress->~QProgressDialog();

}


graphVisualizer::graphVisualizer(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

    this->setParent(parent);

    db = QSqlDatabase::addDatabase("QMYSQL", "graph_mysql_connection");
    db.setHostName(DB_host);
    db.setDatabaseName(DB_name);
    db.setUserName(DB_user);
    db.setPassword(DB_pass);
   // db.transaction();

    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";

    this->graph_selected = 0;
    this->timestamp_init = 0;
    this->timestamp_end = 0;
    this->clear_who_vectors();
    this->filter_port_begin = 1;
    this->filter_port_end = 1024;


    is_selection_mode_opengl = false;
    //this->setMouseTracking(true);

    selectBuf = (GLuint*) malloc(sizeof(GLuint) * 2000000);
    bufsize = sizeof(GLuint) * 2000000;


    this->selected_obj = -1;
    this->selected_world_node = -1;
    this->selected_world_leaf = -1;


    this->timer  = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint_slot()));
    timer->setInterval(100);



    this->setFocusPolicy( Qt::StrongFocus);
    this->setEnabled(true);
    this->graph_over_map = false;


    this->wheel_size = 0.001;

    graph_with_bg = true;


}




graphVisualizer::~graphVisualizer()
{
    this->clear_who_vectors();
    db.close();

    this->deleteTexture(texture[0]);
    this->t.~QImage();

}





std::vector<uint32_t> graphVisualizer::ip_in_same_point(uint32_t ip)
{

    double ip_pos_x, ip_pos_y;

    search_world_position_from_name(ip, &ip_pos_x, &ip_pos_y);


    std::vector <uint32_t> ip_same_addr;

    for (int i=0; i<node_name_list.size(); i++)
    {
        if ( fabs(ip_pos_x - this->node_pos_x[i]) + fabs(ip_pos_y - this->node_pos_y[i]) < 0.001 )
            ip_same_addr.push_back(node_name_list[i]);
    }

    return ip_same_addr;

}


void graphVisualizer::initializeGL()
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable( GL_TEXTURE_2D );
    glLoadIdentity();
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    glTranslatef(0.0f, 0.0f, -GL_H/2);


    texture[0] = 255;
    texture[0] = bindTexture(QPixmap(QString(":/images/Mollweide_Projection.jpg")), GL_TEXTURE_2D, GL_RGB);

    gllist = glGenLists(1);
    is_gllist = false;

    if (GL_H != GL_W) {
        qFatal("[graphvisualizer] look at the define: GL_H and GL_W MUST BE EQUAL!!!!");
    }

    glDisable( GL_TEXTURE_2D );

 }



void graphVisualizer::resizeGL(int width, int height)
 {
    this->width = width;
    this->height = height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
 }




void graphVisualizer::search_position_from_name(uint32_t node1name, double * pos_x, double * pos_y)
{
    for (int i=0; i<this->node_name_list.size(); i++)
    {
        if (node1name == this->node_name_list[i] )
        {
            *pos_x = this->node_pos_x[i];
            *pos_y = this->node_pos_y[i];
            return;
        }
    }
}


void graphVisualizer::search_position_from_names(uint32_t node1name, uint32_t node2name, double * pos_1_x, double * pos_1_y, double * pos_2_x, double * pos_2_y)
{
    search_position_from_name(node1name, pos_1_x, pos_1_y);
    search_position_from_name(node2name, pos_2_x, pos_2_y);
    return;
}


void graphVisualizer::search_world_position_from_name(uint32_t node1name, double * pos_x, double * pos_y)
{
    for (int i=0; i<this->node_name_list.size(); i++)
    {
        if (node1name == this->node_name_list[i] )
        {
            *pos_x = this->world_node_pos_x[i];
            *pos_y = this->world_node_pos_y[i];
            return;
        }

    }

}

void graphVisualizer::search_world_position_from_names(uint32_t node1name, uint32_t node2name, double * pos_1_x, double * pos_1_y, double * pos_2_x, double * pos_2_y)
{
    search_world_position_from_name(node1name, pos_1_x, pos_1_y);
    search_world_position_from_name(node2name, pos_2_x, pos_2_y);
    return;
}



void graphVisualizer::mousePressEvent(QMouseEvent *event)
{
    this->click.setX(event->x());
    this->click.setY(event->y());

    //this->deselectIPpositions();

    this->is_selection_mode_opengl = true;
    is_gllist = false;

    //timer->start();
   repaint();
}


void graphVisualizer::mouseMoveEvent(QMouseEvent *event)
{

    /*
    if (graph_over_map == false)
    {
        this->click.setX(event->x());
        this->click.setY(event->y());
        this->is_selection_mode_opengl = true;

        //timer->start();
        //repaint();
        emit updateGL();
    }
    */

}



void graphVisualizer::deselectIPpositions()
{
    this->selected_world_leaf = -1;
    this->selected_world_node = -1;
}



void graphVisualizer::wheelEvent(QWheelEvent *event)
{

    if (event->orientation() == Qt::Vertical)
    {
        wheel_size += (event->delta())/(8.0*15);
        if (wheel_size<0)
            wheel_size>0;
    }
    else
    {
        wheel_angle += (event->delta())/(8.0*15*2*PI);
    }

    event->accept();
    repaint();
}



int graphVisualizer::search_index_ip_in_wip(worldIPposition * p_wip, uint32_t ip)
{
    for (int i=0; i<p_wip->ip.size(); i++)
        if (p_wip->ip[i] == ip)
            return i;
}


void graphVisualizer::paintGL_world_graph(int selected_obj)
{
    qWarning() << "selected_obj" << selected_obj ;



    int n_nodi = IPpositions.size();
    int n_links = this->who_1_orig.size();


    if (selected_obj<0)
    {
        this->deselectIPpositions();
        this->selected_world_node = -1;
        this->selected_world_leaf = -1;
        this->wheel_angle = 0.0f;
        this->wheel_size = 0.0f;
    }



    if ((selected_obj>=0) && (selected_obj < n_nodi))
        {
            this->deselectIPpositions();
            this->selected_world_node = selected_obj;
            this->selected_world_leaf = -1;
        }

        else if ( (selected_obj>=0) && ( selected_world_node >= 0) && (selected_obj - n_nodi-n_links>=0) )
        {
            this->selected_world_leaf = selected_obj - n_nodi-n_links;
        }


    qWarning() << n_nodi << n_links;
    qWarning() << selected_obj;
    qWarning() << this->selected_world_node;
    qWarning() << this->selected_world_leaf;





    bool half_size = false;
    if ( this->selected_world_node >= 0)
    {
        half_size = true;
    }

    if (this->is_selection_mode_opengl == false && graph_with_bg == true)
    {
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);

        if (half_size == false)
            this->drawTexture(QRectF(QPointF(-5,5), QPointF(5,-5)), texture[0], GL_TEXTURE_2D);
        else
            this->drawTexture(QRectF(QPointF(-5,5), QPointF(0,-5)), texture[0], GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_2D);
        glDisable (GL_LIGHTING);
        glColor3f (1.0, 1.0, 1.0);
    }




    glColor4f(1.0f,1.0f,1.0f,0.5f);
    //this->renderBitmapString("Press SPACE to visualize the classical graph,", 3.0f, 4.90f);
    //this->renderBitmapString("B to remove the backfround, H to remove the extra-links", 3.0f, 4.80f);





    //qWarning() << selected_obj << selected_world_node << selected_world_leaf;


    glPointSize(1.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);

    double pos_1_x, pos_1_y, pos_2_x, pos_2_y;


    for (int c=0; c<this->who_1_orig.size(); c++)
    {

        worldIPposition * p_wip_1 = &(this->IPpositions[who_1_world_pos_pointer[c]]);
        worldIPposition * p_wip_2 = &(this->IPpositions[who_2_world_pos_pointer[c]]);


        if (link_hidden == true)
        {
            if ( (who_1_world_pos_pointer[c] != selected_world_node) || (who_2_world_pos_pointer[c] != selected_world_node))
                continue;
        }



        glPushMatrix();
        glPushName(c+this->IPpositions.size());




        int max_ct = this->max_links;

        glLineWidth(3.0 * (float) count_links[c] /(float)max_ct);

        double off_1_x=0, off_1_y=0, off_2_x=0, off_2_y=0;



        if (who_1_world_pos_pointer[c] == selected_world_node)
        {
            float index = (float) search_index_ip_in_wip(p_wip_1, who_1_orig[c]);
            off_1_x = this->wheel_size*cos(2*PI*wheel_angle +    2*PI*index/p_wip_1->ip.size());
            off_1_y = this->wheel_size*sin(2*PI*wheel_angle +    2*PI*index/p_wip_1->ip.size());
            pos_1_x = 0.5f;
            pos_1_y = 0.0f;

        }
        else
        {
            pos_1_x = p_wip_1->x;
            pos_1_y = p_wip_1->y;
        }




        if (who_2_world_pos_pointer[c] == selected_world_node)
        {
            float index = (float) search_index_ip_in_wip(p_wip_2, who_2_orig[c]);
            off_2_x = this->wheel_size*cos(2*PI*wheel_angle +    2*PI*index/p_wip_2->ip.size());
            off_2_y = this->wheel_size*sin(2*PI*wheel_angle +    2*PI*index/p_wip_2->ip.size());
            pos_2_x = 0.5f;
            pos_2_y = 0.0f;
        }
        else
        {
            pos_2_x = p_wip_2->x;
            pos_2_y = p_wip_2->y;
        }


        QColor * color_init = new QColor();

        if (selected_obj < 0)
        {
            color_init->setRgbF(1.0, 0.0, 0.0, 0.8f);
        }
        else if (c+this->IPpositions.size() == selected_obj)
        {
            color_init->setRgbF(1.0, 0.0, 0.0, 0.8f);
        }
        else
        {
            color_init->setRgbF(0.5, 0.0, 0.0, 0.8f);
        }


        float x_pos = bound(GL_W/2*pos_1_x+off_1_x, -0.9*GL_W/2, 0.9*GL_W/2);
        float y_pos = bound(GL_H/2*pos_1_y+off_1_y, -0.9*GL_H/2, 0.9*GL_H/2);


        QPointF * centre_init;
        QPointF * lateral_init;

        if (half_size == false)
        {
            centre_init = new QPointF(bound(GL_W/2*pos_1_x, -0.9*GL_W/2, 0.9*GL_W/2), bound(GL_H/2*pos_1_y, -0.9*GL_H/2, 0.9*GL_H/2));
            lateral_init = new QPointF(bound(GL_W/2*pos_1_x+off_1_x, -0.9*GL_W/2, 0.9*GL_W/2), bound(GL_H/2*pos_1_y+off_1_y, -0.9*GL_H/2, 0.9*GL_H/2));
        }
        else if (half_size == true && who_1_world_pos_pointer[c] == selected_world_node)
        {
            centre_init = new QPointF(bound(GL_W/2*pos_1_x, 0.0, 0.9*GL_W/2), bound(GL_H/2*pos_1_y, -0.9*GL_H/2, 0.9*GL_H/2));
            lateral_init = new QPointF(bound(GL_W/2*pos_1_x+off_1_x, 0.0, 0.9*GL_W/2), bound(GL_H/2*pos_1_y+off_1_y, -0.9*GL_H/2, 0.9*GL_H/2));
        }
        else
        {
            centre_init = new QPointF(bound(-GL_W/4+GL_W/4*pos_1_x, -0.9*GL_W/2, 0.0), bound(GL_H/2*pos_1_y, -0.9*GL_H/2, 0.9*GL_H/2));
            lateral_init = new QPointF(bound(-GL_W/4+GL_W/4*pos_1_x+off_1_x, -0.9*GL_W/2, 0.0), bound(GL_H/2*pos_1_y+off_1_y, -0.9*GL_H/2, 0.9*GL_H/2));
        }



        QColor * color_end = new QColor();

        if (selected_obj < 0)
            color_end->setRgbF(0.0, 1.0, 0.0, 0.8f);
        else if (c+this->IPpositions.size() == selected_obj)
            color_end->setRgbF(0.0, 1.0, 0.0, 0.8f);
        else
            color_end->setRgbF(0.0, 0.5, 0.0, 0.8f);



        x_pos = bound(GL_W/2*pos_2_x+off_2_x, -0.9*GL_W/2, 0.9*GL_W/2);
        y_pos = bound(GL_H/2*pos_2_y+off_2_y, -0.9*GL_H/2, 0.9*GL_H/2);



        QPointF * centre_end;
        QPointF * lateral_end;


        if (half_size == false)
        {
            centre_end = new QPointF(bound(GL_W/2*pos_2_x, -0.9*GL_W/2, 0.9*GL_W/2), bound(GL_H/2*pos_2_y, -0.9*GL_H/2, 0.9*GL_H/2));
            lateral_end  = new QPointF(bound(GL_W/2*pos_2_x+off_2_x, -0.9*GL_W/2, 0.9*GL_W/2), bound(GL_H/2*pos_2_y+off_2_y, -0.9*GL_H/2, 0.9*GL_H/2));
        }
        else if (half_size == true && who_2_world_pos_pointer[c] == selected_world_node)
        {
            centre_end = new QPointF(bound(GL_W/2*pos_2_x, 0.0, 0.9*GL_W/2), bound(GL_H/2*pos_2_y, -0.9*GL_H/2, 0.9*GL_H/2));
            lateral_end  = new QPointF(bound(GL_W/2*pos_2_x+off_2_x, 0.0, 0.9*GL_W/2), bound(GL_H/2*pos_2_y+off_2_y, -0.9*GL_H/2, 0.9*GL_H/2));
        }
        else
        {
            centre_end = new QPointF(bound(-GL_W/4+GL_W/4*pos_2_x, -0.9*GL_W/2, 0.0), bound(GL_H/2*pos_2_y, -0.9*GL_H/2, 0.9*GL_H/2));
            lateral_end  = new QPointF(bound(-GL_W/4+GL_W/4*pos_2_x+off_2_x, -0.9*GL_W/2, 0.0), bound(GL_H/2*pos_2_y+off_2_y, -0.9*GL_H/2, 0.9*GL_H/2));
        }


        glLineWidth(5.0 * (float) count_links[c] /(float)max_ct);

        int num_pts = 4;
        int iters = 32;
        GLfloat * control_points;
        control_points = new GLfloat[num_pts*3];

        control_points[0] = lateral_init->x();
        control_points[1] = lateral_init->y();
        control_points[2] = 0.0f;

        control_points[3] = centre_init->x();
        control_points[4] = centre_init->y();
        control_points[5] = 0.0f;

        control_points[6] = centre_end->x();
        control_points[7] = centre_end->y();
        control_points[8] = 0.0f;

        control_points[9] = lateral_end->x();
        control_points[10] = lateral_end->y();
        control_points[11] = 0.0f;




        GLfloat * color_points = new GLfloat[2*4];
        color_points[0] = color_init->redF();
        color_points[1] = color_init->greenF();
        color_points[2] = color_init->blueF();
        color_points[3] = color_init->alphaF();

        color_points[4] = color_end->redF();
        color_points[5] = color_end->greenF();
        color_points[6] = color_end->blueF();
        color_points[7] = color_end->alphaF();

        glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, num_pts, &control_points[0]);
        glMap1f(GL_MAP1_COLOR_4, 0.0f, 1.0f, 4, 2, &color_points[0]);

        glEnable(GL_MAP1_VERTEX_3);
        glEnable(GL_MAP1_COLOR_4);
        glBegin(GL_LINE_STRIP);
        for(int k = 0; k <= iters; k++)
        {
            glEvalCoord1f((GLfloat)k / (GLfloat)iters);
        }
        glEnd();
        glDisable(GL_MAP1_VERTEX_3);
        glDisable(GL_MAP1_COLOR_4);
        delete[] control_points;
        delete[] color_points;



        delete centre_init;
        delete lateral_init;
        delete centre_end;
        delete lateral_end;

        delete color_init;
        delete color_end;



        glPopName();
        glPopMatrix();

        glLineWidth(1.0);

    }


    glPointSize(5.0f);
    for (int c=0; c<IPpositions.size(); c++)
    {
        glPushMatrix();
        glPointSize(2.0f);


        if (selected_world_node < 0)
            glColor3f(1.0f,1.0f,1.0f);
        else if (c == selected_world_node)
            glColor3f(0.0f,0.0f,1.0f);
        else
            glColor3f(0.6f,0.6f,0.6f);


        worldIPposition * p_wip = &(this->IPpositions[c]);

        if (c != selected_world_node)
        {
            glPushName(c);
            glPointSize((0.2*p_wip->ip.size())>10.0? 10.0f:(0.2*p_wip->ip.size()));
            glBegin(GL_POINTS);

            if (half_size == false)
                glVertex3f((GL_W/2)*p_wip->x, (GL_H/2)*p_wip->y, 0.0001f);
            else
                glVertex3f(-GL_W/4+GL_W/4*p_wip->x, (GL_H/2)*p_wip->y, 0.0001f);

            glEnd();
            glPopName();
        }
        else
        {

            glColor3f(1.0f,1.0f,0.0f);
            glPointSize((0.2*p_wip->ip.size())>10.0? 10.0f:(0.2*p_wip->ip.size()));
            glBegin(GL_POINTS);

            if (half_size == false)
                glVertex3f((GL_W/2)*p_wip->x, (GL_H/2)*p_wip->y, 0.0001f);
            else
            {
                glVertex3f(-GL_W/4+GL_W/4*p_wip->x, (GL_H/2)*p_wip->y, 0.0001f);

                glPointSize(6.0f);
                glVertex3f(GL_W/4, 0.0f, 0.0001f);
            }

            glEnd();




            if (selected_world_node < 0)
                glColor3f(1.0f,1.0f,1.0f);
            else if (c == selected_world_node)
                glColor3f(0.0f,0.0f,1.0f);
            else
                glColor3f(0.6f,0.6f,0.6f);

            glPointSize(3.0f);

            for (int i=0; i<p_wip->ip.size(); i++)
            {
                glPushName(IPpositions.size()+who_1_orig.size()+i);
                glBegin(GL_POINTS);

                float x_pos = bound((GL_W/2)*0.5f + this->wheel_size*cos(2*PI*wheel_angle +    2*PI*i/p_wip->ip.size()), 0.0, 0.9*GL_W/2);
                float y_pos = bound((GL_H/2)*0.0f + this->wheel_size*sin(2*PI*wheel_angle +    2*PI*i/p_wip->ip.size()), -0.9*GL_H/2, 0.9*GL_H/2);
                glVertex3f(x_pos, y_pos, 0.0001f);
                glEnd();
                glPopName();
            }
        }

        glPopMatrix();

    }


    glLineWidth(1.0);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


void graphVisualizer::paintGL_graph()
{


    glDisable(GL_TEXTURE_2D);
    glDisable (GL_LIGHTING  );
    glColor3f (1.0, 1.0, 1.0);


    //glColor4f(1.0f,1.0f,1.0f,0.5f);
    //this->renderBitmapString("Press SPACE to visualize the world graph", 3.0f, 4.9f);

    int max_ct = this->max_links;


    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);

    double pos_1_x, pos_1_y, pos_2_x, pos_2_y;

    for (int c=0; c<this->who_1_orig.size(); c++)
    {

        glPushMatrix();
        glPushName(c+this->node_name_list.size());



        glLineWidth(3.0 * (float) count_links[c] /(float)max_ct);

        pos_1_x = this->who_1_orig_pos_x[c];
        pos_1_y = this->who_1_orig_pos_y[c];
        pos_2_x = this->who_2_orig_pos_x[c];
        pos_2_y = this->who_2_orig_pos_y[c];

        glBegin(GL_LINES);

        if (this->selected_obj < 0)
        {
            glColor4f(1.0, 0.0, 0.0, 1.0f);
            glLineWidth(3.0 * (float) count_links[c] /(float)max_ct);
        }
        else if (c+this->node_name_list.size() == this->selected_obj)
        {
            glColor4f(1.0, 0.0, 0.0, 1.0f);
            glLineWidth(5.0 * (float) count_links[c] /(float)max_ct);
        }
        else
        {
            glColor4f(0.5, 0.0, 0.0, 1.0f);
            glLineWidth(3.0 * (float) count_links[c] /(float)max_ct);
        }


        glVertex3f(-GL_W/2+1 + (GL_W-2)*pos_1_x, -GL_H/2+1 + (GL_H-2)*pos_1_y, 0.0f);



        if (this->selected_obj < 0)
            glColor4f(0.0, 1.0, 0.0, 1.0f);
        else if (c+this->node_name_list.size() == this->selected_obj)
            glColor4f(0.0, 1.0, 0.0, 1.0f);
        else
            glColor4f(0.0, 0.5, 0.0, 1.0f);


        glVertex3f(-GL_W/2+1 + (GL_W-2)*pos_2_x, -GL_H/2+1 + (GL_H-2)*pos_2_y, 0.0f);
        glEnd();


        glPopName();
        glPopMatrix();

        glLineWidth(1.0);


    }




    glPointSize(5.0f);
    for (int c=0; c<this->node_name_list.size(); c++)
    {
        glPushMatrix();
        glPushName(c);


        if (this->selected_obj < 0)
            glColor3f(1.0f,1.0f,1.0f);
        else if (c== this->selected_obj)
            glColor3f(1.0f,1.0f,1.0f);
        else
            glColor3f(0.5f,0.5f,0.5f);

        glBegin(GL_POINTS);
        glVertex3f(-GL_W/2+1 + (GL_W-2)*this->node_pos_x[c], -GL_H/2+1 + (GL_H-2)*this->node_pos_y[c], 0.0001f);
        glEnd();

        glPopName();
        glPopMatrix();

    }
    glPointSize(1.0f);


    glLineWidth(1.0);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);




}


int graphVisualizer::max_count_links()
{
    int m = -1;

    for (size_t i = 0; i<this->count_links.size(); i++)
        if (this->count_links[i] > m)
            m = this->count_links[i];

    return m;
}



void graphVisualizer::writeLabels()
{

    glColor4f(1.0, 0.0, 0.0, 1.0f);
    this->renderBitmapString("SOURCE", 4, -4.5);
    glColor4f(0.0, 1.0, 0.0, 1.0f);
    this->renderBitmapString("DESTINATION", 4, -4.85);


    if (this->selected_obj < 0 || this->node_name_list.empty() || this->who_1_orig.empty() )
        return;


    glColor4f(1.0, 1.0, 1.0, 1.0f);

    // selected node
    if (this->selected_obj < this->node_name_list.size()) {
        this->renderBitmapString(std::string("Click onto node: ") + ip_uint32_to_stdstring(this->node_name_list[this->selected_obj]), -2, -4.5);
        int in, out;
        this->node_cardinality(this->node_name_list[this->selected_obj], in, out);
        //this->renderBitmapString(std::string("CARDINALITY   IN#: ") +  std::string(int2str(in)) + std::string("  OUT#: ") + std::string(int2str(out)), -2, -4.85);
    }
    // selected link
    else
    {
        int c = this->selected_obj - this->node_name_list.size();
        this->renderBitmapString(ip_uint32_to_stdstring(this->who_1_orig[c]) + " and "+ ip_uint32_to_stdstring(this->who_2_orig[c]), -2, -4.5);
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}



void graphVisualizer::writeWorldLabels()
{


    glColor4f(1.0, 0.0, 0.0, 1.0f);
    this->renderBitmapString("SOURCE", 4, -4.5);
    glColor4f(0.0, 1.0, 0.0, 1.0f);
    this->renderBitmapString("DESTINATION", 4, -4.85);


    if (this->selected_obj < 0 || this->node_name_list.empty() || this->who_1_orig.empty() )
        return;


    int n_nodi = IPpositions.size();
    int n_links = this->who_1_orig.size();

    glColor4f(1.0, 1.0, 1.0, 1.0f);

    // selected node
    if (selected_obj < n_nodi)
    {
        int ip_same_point = IPpositions[selected_world_node].ip.size();
        this->renderBitmapString(std::string("Distinct IP addresses in this location: ") + int2stdstr(ip_same_point), -4.5, -4.5);


        db.setDatabaseName(DB_ip2coord);
        if (!db.open()) qDebug() << "Failed to connect to root mysql admin";
        QString query = QString("SELECT country_code, country_name, region_code, region_name, city, zipcode from ip_group_city where ip_start<=") + \
                        QString(uint2str(IPpositions[selected_world_node].ip[0])) + QString(" order by ip_start desc limit 1;");

        QSqlQuery q = db.exec(query);
        QString result;
        if (q.size() > 0)
        {
            q.seek(-1);
            while ( q.next() )
            {
                result = q.value(0).toString() +" "+ q.value(1).toString() +" "+ q.value(2).toString() +" "+ q.value(3).toString() +" "+ q.value(4).toString() +" "+ q.value(5).toString();
                break;
            }
        }

        this->renderBitmapString(result.toStdString(), -4.5, -4.80);

    }

    // selected leaf
    else if (( selected_world_node >= 0) && (selected_obj - n_nodi-n_links>=0))
    {

        int in, out;
        this->node_cardinality(IPpositions[selected_world_node].ip[selected_world_leaf], in, out);
        this->renderBitmapString(std::string("Click onto node: ") + ip_uint32_to_stdstring(IPpositions[selected_world_node].ip[selected_world_leaf]) + std::string("   CARDINALITY   IN#: ") +  \
                                 std::string(int2str(in)) + std::string("  OUT#: ") + std::string(int2str(out)), -4.5, -4.5);


        db.setDatabaseName(DB_ip2coord);
        if (!db.open()) qDebug() << "Failed to connect to root mysql admin";
        QString query = QString("SELECT country_code, country_name, region_code, region_name, city, zipcode from ip_group_city where ip_start<=") + \
                        QString(uint2str(IPpositions[selected_world_node].ip[selected_world_leaf])) + QString(" order by ip_start desc limit 1;");

        qWarning()<< query;
        qWarning() << IPpositions[selected_world_node].ip[selected_world_leaf];
        QSqlQuery q = db.exec(query);
        QString result;
        if (q.size() > 0)
        {
            q.seek(-1);
            while ( q.next() )
            {
                result = q.value(0).toString() +" "+ q.value(1).toString() +" "+ q.value(2).toString() +" "+ q.value(3).toString() +" "+ q.value(4).toString() +" "+ q.value(5).toString();
                break;
            }
        }

        this->renderBitmapString(result.toStdString(), -4.5, -4.80);


    }
    // selected link
    else if (selected_obj-n_nodi < n_links)
    {
        int c = this->selected_obj - n_nodi;
        this->renderBitmapString(ip_uint32_to_stdstring(this->who_1_orig[c]) + " and "+ ip_uint32_to_stdstring(this->who_2_orig[c]), -4.5, -4.5);

        db.setDatabaseName(DB_ip2coord);
        if (!db.open()) qDebug() << "Failed to connect to root mysql admin";
        QString query = QString("SELECT country_code, country_name, region_code, region_name, city, zipcode from ip_group_city where ip_start<=") + \
                        QString(uint2str(this->who_1_orig[c])) + QString(" order by ip_start desc limit 1;");
        QSqlQuery q = db.exec(query);
        QString result1, result2;
        if (q.size() > 0)
        {
            q.seek(-1);
            while ( q.next() )
            {
                result1 = q.value(0).toString() +" "+ q.value(1).toString() +" "+ q.value(2).toString() +" "+ q.value(3).toString() +" "+ q.value(4).toString() +" "+ q.value(5).toString();
                break;
            }
        }
        query = QString("SELECT country_code, country_name, region_code, region_name, city, zipcode from ip_group_city where ip_start<=") + \
                QString(uint2str(this->who_2_orig[c])) + QString(" order by ip_start desc limit 1;");
        q = db.exec(query);
        if (q.size() > 0)
        {
            q.seek(-1);
            while ( q.next() )
            {
                result2 = q.value(0).toString() +" "+ q.value(1).toString() +" "+ q.value(2).toString() +" "+ q.value(3).toString() +" "+ q.value(4).toString() +" "+ q.value(5).toString();
                break;
            }
        }

        this->renderBitmapString(ip_uint32_to_stdstring(this->who_1_orig[c]) + " " + result1.toStdString(), -4.5, -4.80);
        this->renderBitmapString(ip_uint32_to_stdstring(this->who_2_orig[c]) + " " + result2.toStdString(), -4.5, -4.93);

    }
    else
        qWarning("Alberto's error!");

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}



void graphVisualizer::renderBitmapString(std::string txt, float x, float y)
{
    glRasterPos3f(x, y,0);
    for (size_t c = 0; c<txt.length(); c++) {
      glutBitmapCharacter(GLUT_FONT, txt[c]);
    }
}




void graphVisualizer::paintGL()
{


    if (texture[0]==255) return;

    glMatrixMode(GL_MODELVIEW);


    if (this->is_selection_mode_opengl)
        this->startPicking(click.x(), click.y());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    if (this->graph_over_map == false)
    {
        paintGL_graph();
        writeLabels();
    }
    else
    {
        paintGL_world_graph(this->selected_obj);
        writeWorldLabels();
    }



    if (this->is_selection_mode_opengl)
        this->stopPicking();


    glFlush();

    glutSwapBuffers();

}


void graphVisualizer::draw_graph_feature(int feat, int port_init, int port_end)
{
    this->graph_selected = feat;
    this->filter_port_begin = port_init;
    this->filter_port_end = port_end;

    fetch_data();
    create_lists();
    /*
    QFuture<void> future = QtConcurrent::run(this, &graphVisualizer::computeLayout);
    QFuture<void> future2 = QtConcurrent::run(this, &graphVisualizer::computeLayout_worldmap);
    future.waitForFinished();
    future2.waitForFinished();
    */
    computeLayout();
    computeLayout_worldmap();

    is_gllist = false;
    repaint();

}



void graphVisualizer::time_clicked(int timestamp_init, int timestamp_end)
{

    this->timestamp_init = timestamp_init;
    this->timestamp_end = timestamp_end;
    if (timestamp_init == timestamp_end)
        this->timestamp_end += DB_DELTA_HIST;

    repaint();
}


void graphVisualizer::node_cardinality(uint32_t name, int & in, int & out)
{
    in = 0;
    out = 0;

    for (size_t i = 0; i< this->who_1_orig.size(); i++)
    {
        if (name == who_1_orig[i])
            out++;
    }

    for (size_t i = 0; i< this->who_2_orig.size(); i++)
    {
        if (name == who_2_orig[i])
            in++;
    }


}


// Fetch the histograms of the features for every timestamp between init and end
void graphVisualizer::fetch_data()
{

    qWarning() << "fetch_data() BEGIN";

    this->clear_who_vectors();


    if (this->isHidden() == true)
        return;


    db.setDatabaseName(DB_name);
    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";


    QProgressDialog * progress = new QProgressDialog(this);
    progress->setLabelText("Fetching data from MySql DB");
    progress->setRange(0, 2);
    progress->setMinimumDuration (1);
    progress->setValue(1);
    progress->setModal(true);

    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";
    QSqlQuery q;

    QString qstr;

    switch (this->graph_selected) {
        case 0:
        qstr = QString("SELECT sip, dip, count(0) FROM ") + QString(DB_PCAP_TABLE) + QString(" WHERE proto = 6 AND (( sport>=")  + QString(int2str(this->filter_port_begin)) + QString(" and sport<= ") + QString(int2str(this->filter_port_end)) + \
               QString (") OR ( dport>=") + QString(int2str(this->filter_port_begin)) + QString(" and dport<= ") + QString(int2str(this->filter_port_end)) + QString(")) AND timestamp >= ") + QString(int2str(this->timestamp_init)) + \
               QString(" AND timestamp <= ") + QString(int2str(this->timestamp_end)) + QString(" GROUP BY sip, dip;");
        break;
        case 1:
        qstr = QString("SELECT  sip, dip, count(0) FROM ") + QString(DB_PCAP_TABLE) + QString(" WHERE proto =17 AND (( sport>=")  + QString(int2str(this->filter_port_begin)) + QString(" and sport<= ") + QString(int2str(this->filter_port_end)) + \
               QString (") OR ( dport>=") + QString(int2str(this->filter_port_begin)) + QString(" and dport<= ") + QString(int2str(this->filter_port_end)) + QString(")) AND timestamp >= ") + QString(int2str(this->timestamp_init)) + \
               QString(" AND timestamp <= ") + QString(int2str(this->timestamp_end)) + QString(" GROUP BY sip, dip;");
        break;
        case 2:
        qstr = QString("SELECT sip, dip, count(0) FROM ") + QString(DB_PCAP_TABLE) + QString(" WHERE proto = 1 AND timestamp >= ") + QString(int2str(this->timestamp_init)) + \
               QString(" AND timestamp <= ") + QString(int2str(this->timestamp_end)) + QString(" GROUP BY sip, dip;");
        break;
    default:
        qWarning() << "ERROR IN QUERY GRAPH NUMBER!!!!!";
        break;

    }

    qWarning() << qstr;


    q = db.exec(qstr);

    if (q.size() > 0)
    {
        q.seek(-1);
        while ( q.next() )
        {

            who_1_orig.push_back(ip_stdstring_to_uint32(q.value(0).toString().toStdString()));
            who_2_orig.push_back(ip_stdstring_to_uint32(q.value(1).toString().toStdString()));

            count_links.push_back(q.value(2).toInt());
        }
    }
    else
    {
        qWarning() <<  "Empty table with query " << qstr;
    }

    this->filter_graph();

    delete progress;

    this->print_vectors();
    std::cout << qstr.toStdString() << std::endl;





    this->max_links = this->max_count_links();



    qWarning() << "fetch_data() END";


}


void graphVisualizer::filter_graph()
{
    for (int i=0; i<who_1_orig.size(); i++)
    {
        if (who_1_orig[i] == 100000)
            continue;

        for (int j=i+1; j<who_2_orig.size(); j++)
        {
            if ( (who_1_orig[i] == who_2_orig[j]) && (who_2_orig[i] == who_1_orig[j]) )
            {
                if (count_links[i] > count_links[j])
                {
                    who_1_orig[j] = 100000;
                    who_2_orig[j] = 100000;
                }
                else
                {
                    who_1_orig[i] = 100000;
                    who_2_orig[i] = 100000;
                }
                break;
            }
        }
    }

    for (int i=0; i<who_1_orig.size(); i++)
    {
        if (who_1_orig[i] == 100000)
        {
            who_1_orig.erase(who_1_orig.begin()+i);
            who_2_orig.erase(who_2_orig.begin()+i);
            count_links.erase(count_links.begin()+i);
            i--;
        }
    }





}



void graphVisualizer::clear_who_vectors()
{
    who_1_orig.clear();
    who_2_orig.clear();

    who_1_orig_pos_x.clear();
    who_1_orig_pos_y.clear();
    who_2_orig_pos_x.clear();
    who_2_orig_pos_y.clear();


    IPpositions.clear();
    who_1_world_pos_pointer.clear();
    who_2_world_pos_pointer.clear();

    node_name_list.clear();

    ip_cardinality.clear();


}


void graphVisualizer::print_vectors()
{
    for (size_t i = 0; i< this->who_1_orig.size(); i++)
    {
        std::cout << ip_uint32_to_stdstring(who_1_orig[i]) << " \t";
        std::cout << ip_uint32_to_stdstring(who_2_orig[i]) << " \t";
        std::cout << "  # = " << count_links[i] << std::endl;
    }
}






void graphVisualizer::startPicking(int x, int y)
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
    std::cout << "End Start picking" << std::endl;
 }



void graphVisualizer::stopPicking()
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
    timer->start();
    updateGL();

}


void graphVisualizer::processHits2 (GLint hits, GLuint buffer[], int sw)
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


                 if ((int) *ptr < selected_obj)
                 {
                     selected_obj = (int) *ptr;
                     is_gllist = false;
                 }
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

