#ifndef MATRIX_VISUALIZER_H
#define MATRIX_VISUALIZER_H


#include <QGLWidget>
#include <QSqlDatabase>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <vector>


class MatrixVisualizer : public QGLWidget
{
    Q_OBJECT
public:
    MatrixVisualizer(QWidget *parent = 0);
    ~MatrixVisualizer();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mouseMoveEvent(QMouseEvent *event);


public slots:
    void matrix_is_active(bool yes) {yes==true? this->setHidden(false): this->setHidden(true); this->repaint();}
    void draw_matrix_on_signal(int proto, int feat_x, int feat_y, int feat_height, int port_filter_begin, int port_filter_end);
    void repaint_slot() {repaint(); timer->stop();}
    void time_clicked(int, int);


private:
    int height;
    int width;

    QSqlDatabase db;
    QTimer * timer;

    int timestamp_init;
    int timestamp_end;

    float max_height;



    std::vector<uint32_t> pos_r;
    std::vector<uint32_t> pos_c;




    void fetch_data(int proto, int feat_x, int feat_y, int feat_height, int port_filter_begin, int port_filter_end);
    void clean_who_vectors();
    void build_matrix();
    void create_nodes_vector();
    int value_to_index(uint32_t value, std::vector<uint32_t> stringvector);
    void paintGL_matrix();
    void search_indexes(uint32_t from, uint32_t to, int & row, int & col);
    int maxheight();
    void renderBitmapString(std::string txt, float x, float y);
    void writeLabels();

    std::vector<uint32_t> who_1;
    std::vector<uint32_t> who_2;
    std::vector<int> link_height;

    std::vector<uint32_t> node1_name_vector;
    std::vector<uint32_t> node2_name_vector;
    std::vector<uint32_t> ranking_1;
    std::vector<uint32_t> ranking_2;


    std::string axes_x_name;
    std::string axes_y_name;




    QPoint click;
    bool is_selection_mode_opengl;
    long bufsize;
    GLuint * selectBuf;
    void startPicking(int x, int y);
    void stopPicking();
    void processHits2 (GLint hits, GLuint buffer[], int sw);
    int selected_obj;


};

#endif // MATRIX_VISUALIZER_H
