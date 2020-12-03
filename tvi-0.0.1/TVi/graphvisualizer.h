#ifndef GRAPHVISUALIZER_H
#define GRAPHVISUALIZER_H

#include <QGLWidget>
#include <QSqlDatabase>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <vector>


#include <ogdf/basic/Graph_d.h>




class worldIPposition
{
public:
    ~worldIPposition(){this->ip.clear();}

    double x;
    double y;

    std::vector<uint32_t> ip;
};






class graphVisualizer : public QGLWidget
{
    Q_OBJECT
public:
    graphVisualizer(QWidget *parent = 0);
    ~graphVisualizer();


protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);



public slots:
    void graph_is_active(bool yes) {yes==true? this->setHidden(false): this->setHidden(true); this->repaint();}
    void draw_graph_feature(int, int, int);
    void time_clicked(int, int);
    void repaint_slot() {repaint(); timer->stop();}



private:
    QSqlDatabase db;
    QTimer * timer;



    QImage t;
    bool graph_over_map;

    bool link_hidden;



    float wheel_size;
    float wheel_angle;


    int timestamp_init;
    int timestamp_end;


    int filter_port_begin;
    int filter_port_end;
    int graph_selected;

    std::vector<uint32_t> who_1_orig;
    std::vector<uint32_t> who_2_orig;


    std::vector<double> who_1_orig_pos_x;
    std::vector<double> who_1_orig_pos_y;
    std::vector<double> who_2_orig_pos_x;
    std::vector<double> who_2_orig_pos_y;



    std::vector<worldIPposition> IPpositions;
    std::vector<int> who_1_world_pos_pointer;
    std::vector<int> who_2_world_pos_pointer;


    std::vector<uint32_t> node_name_list;


    std::vector<int> ip_cardinality;



    std::vector<double> node_pos_x;
    std::vector<double> node_pos_y;


    std::vector<double> world_node_pos_x;
    std::vector<double> world_node_pos_y;

    std::vector<int> count_links;
    int max_links;



    int height;
    int width;

    void fetch_data();
    void clear_who_vectors();
    void print_vectors();
    void paintGL_graph();
    void paintGL_world_graph(int selected_obj);
    void search_position_from_name(uint32_t node1name, double * pos_x, double * pos_y);
    void search_position_from_names(uint32_t node1name, uint32_t node2name, double * pos_1_x, double * pos_1_y, double * pos_2_x, double * pos_2_y);

    void search_world_position_from_name(uint32_t node1name, double * pos_x, double * pos_y);
    void search_world_position_from_names(uint32_t node1name, uint32_t node2name, double * pos_1_x, double * pos_1_y, double * pos_2_x, double * pos_2_y);

    void node_cardinality(uint32_t name, int & in, int & out);
    int search_index_ip_in_wip(worldIPposition * p_wip, uint32_t ip);


    void create_lists();


    int max_count_links();

    void computeLayout();
    void computeLayout_worldmap();
    void writeLabels();
    void writeWorldLabels();
    void renderBitmapString(std::string txt, float x, float y);


    int selected_world_node;
    int selected_world_leaf;


    bool graph_with_bg;



    QPoint click;
    bool is_selection_mode_opengl;
    long bufsize;
    GLuint * selectBuf;
    void startPicking(int x, int y);
    void stopPicking();
    void processHits2 (GLint hits, GLuint buffer[], int sw);
    int selected_obj;
    GLuint gllist;
    bool is_gllist;

    std::vector<uint32_t> ip_in_same_point(uint32_t ip);
    void filter_graph();


    void deselectIPpositions();


    GLuint texture[1];


};

#endif // GRAPHVISUALIZER_H
