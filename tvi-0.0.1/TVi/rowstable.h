#ifndef ROWSTABLE_H
#define ROWSTABLE_H


#include <vector>
#include <string>
#include <map>




class Table_Cell
{
public:
    int m;
    int n;
};


class RowsTable
{
public:
    RowsTable();
    ~RowsTable();

    bool isActive(){return is_table_active;}
    void setStatus(bool status){is_table_active = status;}
    void InsertValues(std::string value_bin, double value_counter, int r, int c);

    //void CreateCSTable();

    //double GetCSValue(int r, int c) {return CS_counter_VV[r][c];}

    std::string GetBinValue(int r, int c) {return bin_VV[r][c];}
    double GetCounterValue(int r, int c) {return counter_VV[r][c];}


    //double GetMAXCSValue();
    double GetMAXCounter();
    int GetNumCols() {return n_cols+1;}
    int GetNumRows() {return n_rows+1;}

    void ClearAll();
    void PrintSize();
    void PlotTables();
    //void PlotTableCS();

    std::string name;


    bool isPort;
    bool isIP;

    // position of the histogram matrix
    float x,y,w,h;


    // position of the descending-order histogram
    float x1,y1,w1,h1;

    //position of the hilbert map or sorted port hist
    float x2,y2,w2,h2;

    void setPositionOnScreen(float x, float y, float w, float h);
    void setPositionOnScreen_2nd_graph(float x2, float y2, float w2, float h2);
    void setPositionOnScreen_1st_graph(float x1, float y1, float w1, float h1);

    bool pointIsInside(float a, float b);


    std::vector<Table_Cell> selected_cells;
    bool insert_new_selected_cell (int m, int n);
    bool remove_selected_cell (int m, int n);
    void clear_selected_cells() {selected_cells.clear();}
    bool is_cell_selected(int m, int n);


private:
    std::vector<std::vector<std::string> > bin_VV;
    std::vector<std::vector<double> > counter_VV;

    std::vector<std::vector<double> > CS_counter_VV;

    bool is_table_active;

    int n_rows;
    int n_cols;

};

#endif // ROWSTABLE_H
