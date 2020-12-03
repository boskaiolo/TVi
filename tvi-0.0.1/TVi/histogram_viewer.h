#ifndef HISTOGRAM_VIEWER_H
#define HISTOGRAM_VIEWER_H


#include <QGLWidget>
#include <vector>
#include <QSqlDatabase>
#include <QProgressDialog>
#include "rowstable.h"




class HistogramViewer : public QGLWidget
{
    Q_OBJECT
public:
    HistogramViewer(QWidget *parent = 0);
    ~HistogramViewer();


protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void time_clicked(int, int, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);
    void log_hist_clicked(bool);
    void paint_all(void) {repaint();}
    void histogram_is_active(bool yes) {yes==true? this->setHidden(false): this->setHidden(true); this->repaint();}


signals:
    void send_log(QString);
    void selected_timestamps(std::vector<int> ts);
    void repaint_me(void);

private:

    int height;
    int width;
    QSqlDatabase db;
    QPoint click;


    bool one_global_histogram;


    bool log_hist;
    std::vector<int> timestamp_V;

    int timestamp_init;
    int timestamp_end;
    uint32_t timestamp_init_index;
    uint32_t timestamp_end_index;


    RowsTable table_tcp_sip;
    RowsTable table_tcp_sport;
    RowsTable table_tcp_dip;
    RowsTable table_tcp_dport;

    RowsTable table_udp_sip;
    RowsTable table_udp_sport;
    RowsTable table_udp_dip;
    RowsTable table_udp_dport;

    RowsTable table_icmp_sip;
    RowsTable table_icmp_dip;

    int get_n_active_features();


    void fetch_histograms(int timestamp_init, int timestamp_end);
    void fetch_table(std::string query_where, RowsTable * table, int timestamp_init, int timestamp_end, bool isPort, bool isIP, std::string errorbuffer, QProgressDialog * progress);
    void fetch_timestamp_vector();
    void paintGL_text(std::string txt, float x, float y, float r, float g, float b);
    float get_max_height_features();
    int get_max_bincount_features();
    void paintGL_one_whole_histogram(RowsTable * table, float x, float y, float h, float w, float w_scale, float h_scale, float r, float g, float b);
    void paintGL_one_whole_histogram_default();

    void clear_vectors();

    int paintGL_print_features_active(RowsTable * table, int current_hist, float x, float y, float h, float w, float r, float g, float b);
    void paintGL_hist_vector(RowsTable * table, float x, float y, float h, float w, float r, float g, float b);
    void paintGL_hist_vector_matrix(RowsTable * table, float x, float y, float h, float w, float r, float g, float b);
    void paintGL_sorted_port_hist(RowsTable * table, float x, float y, float h, float w, float r, float g, float b);
    void paintGL_Hilbert_map_ip(RowsTable * table, float x, float y, float h, float w);
    void renderBitmapString(std::string txt, float x, float y);
    float click_sorted_port_hist(RowsTable * table, float click_x, float click_y);
    float mouse_over_sorted_port_hist(RowsTable * table, float click_x, float click_y);
    float mouse_over_unsorted_hist(RowsTable * table, float click_x, float click_y);




    bool is_selection_mode_opengl;
    long bufsize;
    GLuint * selectBuf;
    void startPicking(int x, int y);
    void stopPicking();
    void processHits2 (GLint hits, GLuint buffer[], int sw);
    std::vector<int> matrix_timestamp_clicked;
    std::vector<int> timestamp_selected_V;


};


#endif // HISTOGRAM_VIEWER_H
