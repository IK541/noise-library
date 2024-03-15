#include <string>
#include <fstream>
#pragma once

class Grid {
    private:
        double* arr;
        unsigned size_x;
        unsigned size_y;

    public:
        virtual double get(unsigned x, unsigned y);
        virtual void set(unsigned x, unsigned y, double val);
        virtual void set(unsigned x, unsigned y, Grid* grid);

        void fun(double(*f)(double));
        void add(Grid* grid);
        void mul(double coef);
        void normalize(double min, double max);
        friend void fun(Grid* write_grid, Grid* read_grid, double(*f)(double,double));

        void write(int max, std::string file_name);
        friend void colorWrite(Grid* red, Grid* green, Grid* blue, int max, std::string file_name);

    // Add getters and setters
        void setSizeX(unsigned size_x);
        unsigned getSizeX();

        void setSizeY(unsigned size_y);
        unsigned getSizeY();

        void setSize(unsigned size_x, unsigned size_y);

    // Add Constructors and destructor
        Grid(unsigned size_x, unsigned size_y);
        Grid(Grid* grid);
        Grid();

        ~Grid();

        protected:
        void setArr(unsigned index, double value);
        double* getArr();
};

class ToroidalGrid : public Grid {
    using Grid::Grid;
    public:
        double get(unsigned x, unsigned y);
        void set(unsigned x, unsigned y, double val);
        void set(unsigned x, unsigned y, Grid* grid);
};
