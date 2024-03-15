#pragma once
#include <stdexcept>
#include <string>
#include "grid.h"
#include "noise.h"


class GridNormalizationException : public std::exception {
    public:
    GridNormalizationException();
    virtual const char* what() const noexcept override;
};

class GridIndexException : public std::exception {
    public:
    GridIndexException(Grid* grid,unsigned ix,unsigned iy);
    virtual const char* what() const noexcept override;
    private:
    mutable std::string message;
    unsigned bound_x, bound_y, ix, iy;
};

class GridSizeException : public std::exception {
    public:
    GridSizeException(Grid* grid_1, Grid* grid_2);
    virtual const char* what() const noexcept override;
    private:
    mutable std::string message;
    unsigned size_x_1, size_y_1, size_x_2, size_y_2;
};

class NoiseBlockException : public std::exception {
    public:
    NoiseBlockException(BlockNoise* noise, ToroidalGrid* grid);
    virtual const char* what() const noexcept override;
    private:
    mutable std::string message;
    unsigned block_size, grid_size_x, grid_size_y;
};
