#include "grid.h"
#include "exceptions.h"

#include <iostream>

void Grid::setSizeX(unsigned size_x) {
    this->size_x = size_x;
    delete arr;
    this->arr = new double[size_x*size_y]();
}

unsigned Grid::getSizeX() {
    return this->size_x;
}

void Grid::setSizeY(unsigned size_y) {
    this->size_y = size_y;
    delete arr;
    this->arr = new double[size_x*size_y]();
}

unsigned Grid::getSizeY() {
    return this->size_y;
}

void Grid::setSize(unsigned size_x, unsigned size_y) {
    this->size_x = size_x;
    this->size_y = size_y;
    delete arr;
    this->arr = new double[size_x*size_y]();
}

void Grid::setArr(unsigned index, double value) {
    this->arr[index] = value;
}

double* Grid::getArr() {
    return this->arr;
}

double Grid::get(unsigned x, unsigned y){
    if(x<0 || y<0 || x>this->size_x || y>this->size_y) {
        throw GridIndexException(this,x,y);
        // std::cerr << "Error: index out of bounds" << std::endl;
    };
    return this->arr[x*(this->size_y)+y];
}

void Grid::set(unsigned x, unsigned y, double val){
    if(x<0 || y<0 || x>this->size_x || y>this->size_y) {
        throw GridIndexException(this,x,y);
        // std::cerr << "Error: index out of bounds" << std::endl;
    };
    this->arr[x*(this->size_y)+y] = val;
}

void Grid::set(unsigned x, unsigned y, Grid* grid){
    if(x<0 || y<0 || x>this->size_x || y>this->size_y) {
        throw GridIndexException(this,x,y);
        // std::cerr << "Error: index out of bounds" << std::endl;
    };
    for(unsigned i = 0; i < grid->size_x && x+i < this->size_x; ++i)
    for(unsigned j = 0; j < grid->size_y && y+j < this->size_y; ++j)
    this->set(x+i,y+j,grid->get(i,j));
}

void Grid::fun(double(*f)(double)){
    for(unsigned i = 0; i < this->size_x; ++i)
        for(unsigned j = 0; j < this->size_y; ++j)
            this->set(i,j, f(this->get(i,j)));
}

void Grid::add(Grid* grid){
    if(this->getSizeX() != grid->getSizeX() || this->getSizeY() != grid->getSizeY())
        throw GridSizeException(this, grid);
    for(unsigned i = 0; i < this->size_x; ++i)
        for(unsigned j = 0; j < this->size_y; ++j)
            this->set(i,j, this->get(i,j) + grid->get(i,j));
}

void Grid::mul(double coef){
    for(unsigned i = 0; i < this->size_x; ++i)
        for(unsigned j = 0; j < this->size_y; ++j)
            this->set(i,j, coef*(this->get(i,j)));
}

void Grid::normalize(double min, double max){
    double cur_min = this->get(0,0);
    double cur_max = this->get(0,0);
    double cur;
    for(unsigned i = 0; i < this->size_x; ++i)
        for(unsigned j = 0; j < this->size_y; ++j){
            cur = this->get(i,j);
            cur_min = (cur < cur_min ? cur : cur_min);
            cur_max = (cur > cur_max ? cur : cur_max);
        }
    if(cur_min == cur_max) throw GridNormalizationException();
    for(unsigned i = 0; i < this->size_x; ++i)
        for(unsigned j = 0; j < this->size_y; ++j){
            this->set(i,j, ((this->get(i,j) - cur_min) * (max - min) / (cur_max - cur_min) + min));
        }
}

void fun(Grid* write_grid, Grid* read_grid, double(*f)(double,double)){
    if(write_grid->getSizeX() != read_grid->getSizeX() || write_grid->getSizeY() != read_grid->getSizeY())
        throw GridSizeException(write_grid, read_grid);
    for(unsigned i = 0; i < write_grid->size_x; ++i)
        for(unsigned j = 0; j < write_grid->size_y; ++j)
            write_grid->set(i,j, f(write_grid->get(i,j), read_grid->get(i,j)));
}

void Grid::write(int max, std::string file_name){
    std::ofstream ofs((file_name+".pgm"));
    ofs << "P2\n" << this->size_y << " "
    << this->size_x << std::endl
    << max << std::endl;
    for(unsigned i = 0; i < this->size_x; i++){
        for(unsigned j = 0; j < this->size_y; j++){
            ofs << (int)(this->get(i,j)) << " ";
        } ofs << std::endl;
    }
    ofs.close();
    return;
}

void colorWrite(Grid* red, Grid* green, Grid* blue, int max, std::string file_name){
    if(red->getSizeX() != green->getSizeX() || red->getSizeY() != green->getSizeY())
        throw GridSizeException(red, green);
    if(red->getSizeX() != blue->getSizeX() || red->getSizeY() != blue->getSizeY())
        throw GridSizeException(red, blue);
    std::ofstream ofs((file_name+".ppm"));
    ofs << "P3\n" << red->getSizeY() << " "
    << red->getSizeX() << std::endl
    << max << std::endl;
    for(unsigned i = 0; i < red->getSizeX(); i++){
        for(unsigned j = 0; j < red->getSizeY(); j++){
            ofs << (int)(red->get(i,j)) << " " << (int)(green->get(i,j)) << " " << (int)(blue->get(i,j)) << " ";
        } ofs << std::endl;
    }
    ofs.close();
    return;
}

Grid::Grid(unsigned size_x, unsigned size_y){
    this->size_x = size_x;
    this->size_y = size_y;
    this->arr = new double[size_x*size_y]();
}

Grid::Grid(Grid* grid){
    this->size_x = grid->size_x;
    this->size_y = grid->size_y;
    this->arr = new double[size_x*size_y]();
    for(unsigned i = 0; i < size_x; ++i)
    for(unsigned j = 0; j < size_y; ++j)
        this->set(i,j,grid->get(i,j));
}

Grid::Grid(){}

Grid::~Grid(){
    delete this->arr;
}



// TOROIDAL

double ToroidalGrid::get(unsigned x, unsigned y) {
    double* arr = this->getArr();
    return arr[(x%(this->getSizeX()))*(this->getSizeY())+(y%(this->getSizeY()))];
}

void ToroidalGrid::set(unsigned x, unsigned y, double val){
    unsigned index = (x%(this->getSizeX()))*(this->getSizeY())+(y%(this->getSizeY()));
    this->setArr(index, val);
}

void ToroidalGrid::set(unsigned x, unsigned y, Grid* grid){
    for(unsigned i = 0; i < grid->getSizeX(); ++i)
    for(unsigned j = 0; j < grid->getSizeY(); ++j)
    this->set(x+i,y+j,grid->get(i,j));
}

// ToroidalGrid::ToroidalGrid(unsigned size_x, unsigned size_y):Grid(size_x,size_y){}
// ToroidalGrid::ToroidalGrid(Grid* grid):Grid(grid){}
