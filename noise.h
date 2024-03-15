#pragma once
#include <cstdlib>
#include <cmath>
#include "grid.h"

class Noise {
    private:
        unsigned seed;
        double min;
        double max;
    public:
        virtual void apply(Grid* grid) = 0;

    // getters and setters
        void setSeed(unsigned seed);
        unsigned getSeed();

        void setMin(double min);
        double getMin();

        void setMax(double max);
        double getMax();

        void setMinMax(double min, double max);

    // constructors
        Noise();
        Noise(double min, double max);
};

class WhiteNoise : public Noise {
    public:
        void apply(Grid* grid);

        WhiteNoise();
        WhiteNoise(double min, double max);
};



class BlockNoise : public Noise {
    private:
        unsigned block_size;

    public:
        void setBlockSize(unsigned block_size);
        unsigned getBlockSize();

        BlockNoise();
        BlockNoise(double min, double max, unsigned block_size);
};

class ValueNoise : public BlockNoise {
    using BlockNoise::BlockNoise;
    public:
    virtual void apply(Grid* grid);
    protected:
    double value(unsigned x,unsigned y,Grid* lattice);
    inline double smooth_trans_0_1(double x);
    inline double hypotenuse(double x, double y);
};

class ToroidalValueNoise : public ValueNoise {
    using ValueNoise::ValueNoise;
    public:
    void apply(ToroidalGrid* grid);
};

class CellNoise : public BlockNoise {
    using BlockNoise::BlockNoise;
    public:
    virtual void apply(Grid* grid);
    protected:
    double value(unsigned x,unsigned y,Grid* lattice_x,Grid* lattice_y);
    inline double hypotenuse(double x, double y);
};

class ToroidalCellNoise : public CellNoise {
    using CellNoise::CellNoise;
    public:
    void apply(ToroidalGrid* grid);
};


class PerlinNoise : public BlockNoise {
    using BlockNoise::BlockNoise;
    public:
    virtual void apply(Grid* grid);
    protected:
    inline double smoothstep(double x);
    void lerp(Grid* grid, Grid* grid_2, bool horizontal);
};

class ToroidalPerlinNoise : public PerlinNoise {
    using PerlinNoise::PerlinNoise;
    public:
    void apply(ToroidalGrid* grid);
};

class FractalNoise : public Noise {
    public:
        void apply(Grid* grid);

        void setNoise(BlockNoise* noise);
        BlockNoise* getNoise();
        void setOctaves(unsigned octaves);
        unsigned getOctaves();

        FractalNoise();
        FractalNoise(double min, double max, BlockNoise* noise, unsigned octaves);

    private:
        BlockNoise* noise;
        unsigned octaves;
};
