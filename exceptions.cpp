#include "exceptions.h"
#include <sstream>

GridNormalizationException::GridNormalizationException(){};

const char* GridNormalizationException::what() const noexcept{
    return "Grid is non-normalizable as all values are equal\n";
}

GridIndexException::GridIndexException(Grid* grid, unsigned ix, unsigned iy):
message(""),bound_x(grid->getSizeX()),bound_y(grid->getSizeY()),ix(ix),iy(iy)
{};

const char* GridIndexException::what() const noexcept{
    std::stringstream ss;
    ss << "Index out of bounds\n"\
    << "Bounds are: [0," << std::to_string(bound_x)
    << "), [0," << std::to_string(bound_y)
    << ")\nIndex given: [" << std::to_string(ix)
    << "," << std::to_string(iy) << "]\n";
    message = ss.str();
    return message.c_str();
}

GridSizeException::GridSizeException(Grid* grid_1, Grid* grid_2):
size_x_1(grid_1->getSizeX()),size_y_1(grid_1->getSizeY()),
size_x_2(grid_2->getSizeX()),size_y_2(grid_2->getSizeY()){}

const char* GridSizeException::what() const noexcept{
    std::stringstream ss;
    ss << "Two grids given of unequal sizes:\n"\
    << "[" << std::to_string(size_x_1) << ","
    << std::to_string(size_y_1) << "]" << ", "
    << "[" << std::to_string(size_x_2) << ","
    << std::to_string(size_y_2) << "]\n";
    message = ss.str();
    return message.c_str();
}

NoiseBlockException::NoiseBlockException(BlockNoise* noise, ToroidalGrid* grid):
block_size(noise->getBlockSize()),
grid_size_x(grid->getSizeX()),grid_size_y(grid->getSizeY()){}

const char* NoiseBlockException::what() const noexcept{
    std::stringstream ss;
    ss << "Grid dimensions are not integer multiples of block size:\n"\
    << "Block size: " << std::to_string(block_size) << "\n"
    << "Grid dimensions: [" << std::to_string(grid_size_x) << ", "
    << std::to_string(grid_size_y) << "]\n";
    message = ss.str();
    return message.c_str();
}
