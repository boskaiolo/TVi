#include "rowstable.h"

#include <iostream>

RowsTable::RowsTable()
{
    this->ClearAll();
}

RowsTable::~RowsTable()
{
    this->ClearAll();
}


void RowsTable::ClearAll()
{
    for (size_t i = 0; i<bin_VV.size(); i++)
    {
        bin_VV[i].clear();
    }
    bin_VV.clear();

    for (size_t i = 0; i<counter_VV.size(); i++)
    {
        counter_VV[i].clear();
    }
    counter_VV.clear();

    for (size_t i = 0; i<CS_counter_VV.size(); i++)
    {
        CS_counter_VV[i].clear();
    }
    counter_VV.clear();

    n_rows = 0;
    n_cols = 0;
    this->setStatus(false);
    this->isIP = false;
    this->isPort = false;


    counter_VV.push_back(std::vector<double>());
    bin_VV.push_back(std::vector<std::string>());
    counter_VV[0].push_back(0.0);
    bin_VV[0].push_back("");


    selected_cells.clear();

}



bool RowsTable::is_cell_selected(int m, int n)
{
    for (size_t i = 0; i< selected_cells.size(); i++)
    {
        if (  (selected_cells[i].m == m) && (selected_cells[i].n == n)  )
            return true;
    }

    return false;
}

bool RowsTable::insert_new_selected_cell (int m, int n)
{
    Table_Cell tc;
    tc.m = m;
    tc.n = n;
    selected_cells.push_back(tc);




    std::cout << "SELECTED_CELLS: " << std::endl;
    for (size_t i = 0; i<selected_cells.size(); i++ )
        std::cout << m << " " <<  n << std::endl;


    return true;
}

bool RowsTable::remove_selected_cell(int m, int n)
{

    for (size_t i = 0; i< selected_cells.size(); i++)
    {
        if ( (selected_cells[i].m == m) && (selected_cells[i].n == n) )
        {
            selected_cells.erase(selected_cells.begin()+i);
            return true;
        }
    }

    return false;
}


void RowsTable::InsertValues(std::string value_bin, double value_counter, int r, int c)
{

    if (c>=n_cols)
    {
        for (size_t i=0; i<counter_VV.size(); i++)
        {
            counter_VV[i].resize(c+1, 0.0);
            bin_VV[i].resize(c+1, "");
        }
        n_cols = c;
    }



    if (r>=n_rows)
    {
        counter_VV.resize(r+1, std::vector<double>());
        bin_VV.resize(r+1, std::vector<std::string>());

        for (size_t i=0; i<counter_VV.size(); i++)
        {
            counter_VV[i].resize(n_cols+1, 0.0);
            bin_VV[i].resize(n_cols+1, "");
        }
        n_rows = r;
    }


    counter_VV[r][c] = value_counter;
    bin_VV[r][c] = value_bin;

}


void RowsTable::PrintSize()
{
    std::cout << "M = " << n_rows << "\t\tN = " << n_cols << std::endl;
}





/*
void RowsTable::CreateCSTable()
{

    CS_counter_VV.resize(n_rows+1, std::vector<double>());
    for (int i=0; i<= n_rows; i++)
    {
        CS_counter_VV[i].resize(n_cols+1, 0.0);
    }

    for (int j=0; j<=n_cols; j++)
    {
        CS_counter_VV[0][j] = counter_VV[0][j];
    }

    for (int i=1; i<=n_rows; i++)
    {
        for (int j=0; j<=n_cols; j++)
        {
            CS_counter_VV[i][j] = CS_counter_VV[i-1][j] + counter_VV[i][j];
        }
    }
}
*/

double RowsTable::GetMAXCounter()
{

    double maxval = -1;

    for (int j=0; j<=n_cols; j++)
    {
        for (int n=0; n<=n_rows; n++)
        {
            if (counter_VV[n][j] > maxval)
                maxval = counter_VV[n][j];
        }

    }
    return maxval;
}

/*
double RowsTable::GetMAXCSValue()
{


    double maxval = 0;

    for (int j=0; j<=n_cols; j++)
    {
        if (CS_counter_VV[n_rows][j] > maxval)
            maxval = CS_counter_VV[n_rows][j];
    }

    this->PlotTableCS();


    return maxval;
}
*/

void RowsTable::PlotTables()
{
    return;

    std::cout << "COUNTER TABLE" << std::endl;
    for (int i=0; i<=n_rows; i++)
    {
        for (int j=0; j<=n_cols; j++)
            std::cout << counter_VV[i][j] << "\t";

        std::cout << std::endl;
    }


    std::cout << "BIN TABLE" << std::endl;
    for (int i=0; i<=n_rows; i++)
    {
        for (int j=0; j<=n_cols; j++)
            std::cout << bin_VV[i][j] << "\t";

        std::cout << std::endl;
    }

}

/*
void RowsTable::PlotTableCS()
{
    return;

    std::cout << "CS TABLE" << std::endl;
    for (int i=0; i<=n_rows; i++)
    {
        for (int j=0; j<=n_cols; j++)
            std::cout << CS_counter_VV[i][j] << "\t";

        std::cout << std::endl;
    }

}
*/

void RowsTable::setPositionOnScreen(float x, float y, float w, float h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

void RowsTable::setPositionOnScreen_2nd_graph(float x2, float y2, float w2, float h2)
{
    this->x2 = x2;
    this->y2 = y2;
    this->w2 = w2;
    this->h2 = h2;
}

void RowsTable::setPositionOnScreen_1st_graph(float x1, float y1, float w1, float h1)
{
    this->x1 = x1;
    this->y1 = y1;
    this->w1 = w1;
    this->h1 = h1;
}

bool RowsTable::pointIsInside(float x_gl, float y_gl)
{
    if (this->isActive()==false)
        return false;

    if ((x_gl >= x) && (x_gl < (x+w)) && (y_gl >= y) && (y_gl < (y+h)) )
        return true;
    else
        return false;
}
