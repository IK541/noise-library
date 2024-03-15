#include "noise.h"
#include "exceptions.h"

#include <random>

// NOISE
void Noise::setSeed(unsigned seed) {
    this->seed = seed;
}

unsigned Noise::getSeed() {
    return this->seed;
}

void Noise::setMin(double min) {
    this->min = min;
}

double Noise::getMin() {
    return this->min;    
}

void Noise::setMax(double max) {
    this->max = max;
}

double Noise::getMax() {
    return this->max;
}

void Noise::setMinMax(double min, double max){
    this->min = min;
    this->max = max;
}

Noise::Noise() {}

Noise::Noise(double min, double max){
    this->min = min;
    this->max = max;
}

// BLOCK NOISE

void BlockNoise::setBlockSize(unsigned block_size) {
    this->block_size = block_size;
}

unsigned BlockNoise::getBlockSize() {
    return this->block_size;
}

BlockNoise::BlockNoise(){}

BlockNoise::BlockNoise(double min, double max, unsigned block_size):
Noise(min,max),block_size(block_size){}

// WHITE NOISE

WhiteNoise::WhiteNoise(){}

WhiteNoise::WhiteNoise(double min, double max):
Noise(min, max) {}

void WhiteNoise::apply(Grid* grid){
    unsigned seed = this->getSeed();
    for(unsigned i = 0; i < grid->getSizeX(); ++i)
    for(unsigned j = 0; j < grid->getSizeY(); ++j){
        grid->set(i,j,(double)rand_r(&(seed)));
        // this->setSeed(seed);
    }
    grid->normalize(this->getMin(), this->getMax());
}

// VALUE NOISE

inline double ValueNoise::smooth_trans_0_1(double x) {return (2*x-3)*x*x+1;}
inline double ValueNoise::hypotenuse(double x, double y){ return sqrt(x*x+y*y);}

void ValueNoise::apply(Grid* grid){
    unsigned lattice_size_x = (grid->getSizeX()-1)/(this->getBlockSize())+2;
    unsigned lattice_size_y = (grid->getSizeY()-1)/(this->getBlockSize())+2;
    Grid lattice(lattice_size_x, lattice_size_y);
    WhiteNoise lattice_noise(0.0, 1.0);
    lattice_noise.setSeed(this->getSeed());
    lattice_noise.apply(&lattice);
    for(unsigned i = 0; i < grid->getSizeX(); ++i)
    for(unsigned j = 0; j < grid->getSizeY(); ++j){
        grid->set(i,j,value(i,j,&lattice));
    }
    grid->normalize(this->getMin(), this->getMax());
}

double ValueNoise::value(unsigned x,unsigned y,Grid* lattice){
    unsigned lx = x/(this->getBlockSize());
    unsigned ly = y/(this->getBlockSize());
    double bs = (double)(this->getBlockSize());
    double dx = (double)x + 0.5;
    double dy = (double)y + 0.5;
    double dist[4] = {
        hypotenuse((double)(dx-lx*bs),(double)(dy-ly*bs))/bs,
        hypotenuse((double)(dx-lx*bs-bs),(double)(dy-ly*bs))/bs,
        hypotenuse((double)(dx-lx*bs),(double)(dy-ly*bs-bs))/bs,
        hypotenuse((double)(dx-lx*bs-bs),(double)(dy-ly*bs-bs))/bs
    };
    double val[4]={lattice->get(lx,ly),lattice->get(lx+1,ly),lattice->get(lx,ly+1),lattice->get(lx+1,ly+1)};
    double ans = 0;
    for(int i = 0; i < 4; i++) if(dist[i]<1.0) ans += val[i]*smooth_trans_0_1(dist[i]);
    ans /= (dist[0]+dist[1]+dist[2]+dist[3]);
    return ans;
}

void ToroidalValueNoise::apply(ToroidalGrid* grid){
    if(grid->getSizeX() % this->getBlockSize() || grid->getSizeY() % this->getBlockSize())
        throw NoiseBlockException(this, grid);
    unsigned lattice_size_x = (grid->getSizeX()-1)/(this->getBlockSize())+1;
    unsigned lattice_size_y = (grid->getSizeY()-1)/(this->getBlockSize())+1;
    ToroidalGrid lattice(lattice_size_x, lattice_size_y);
    WhiteNoise lattice_noise(0.0, 1.0);
    lattice_noise.setSeed(this->getSeed());
    lattice_noise.apply(&lattice);
    for(unsigned i = 0; i < grid->getSizeX(); ++i)
    for(unsigned j = 0; j < grid->getSizeY(); ++j){
        grid->set(i,j,value(i,j,&lattice));
    }
    grid->normalize(this->getMin(), this->getMax());
}


// CELL NOISE

inline double CellNoise::hypotenuse(double x, double y){ return sqrt(x*x+y*y);}

void CellNoise::apply(Grid* grid){
    unsigned lattice_size_x = (grid->getSizeX()-1)/(this->getBlockSize())+5;
    unsigned lattice_size_y = (grid->getSizeY()-1)/(this->getBlockSize())+5;
    Grid lattice_x(lattice_size_x, lattice_size_y);
    Grid lattice_y(lattice_size_x, lattice_size_y);
    WhiteNoise lattice_noise(0.0,(double)(this->getBlockSize()));
    lattice_noise.setSeed(this->getSeed());
    lattice_noise.apply(&lattice_x);
    lattice_noise.apply(&lattice_y);
    for(unsigned i = 0; i < grid->getSizeX(); ++i)
    for(unsigned j = 0; j < grid->getSizeY(); ++j){
        grid->set(i,j,value(i+2*(this->getBlockSize()),j+2*(this->getBlockSize()),&lattice_x,&lattice_y));
    }
    grid->normalize(this->getMin(), this->getMax());
}

double CellNoise::value(unsigned x,unsigned y,Grid* lattice_x,Grid* lattice_y){
    unsigned lx = x/(this->getBlockSize());
    unsigned ly = y/(this->getBlockSize());
    double bx = (double)(x%(this->getBlockSize()));
    double by = (double)(y%(this->getBlockSize()));

    double bs = (double)(this->getBlockSize());
    double ans = 2*bs;
    double val;
    for(int i = -2; i <= 2; ++i) for(int j = -2; j <= 2; ++j) {
        val = hypotenuse(
            bx-(lattice_x->get(lx+i,ly+j)+i*bs),
            by-(lattice_y->get(lx+i,ly+j)+j*bs)
        );
        ans = (val < ans ? val : ans);
    }
    if(ans > bs){
        int x_list[] = {-1,0,1,2,2,2,1,0,-1,-2,-2,-2};
        int y_list[] = {-2,-2,-2,-1,0,1,2,2,2,1,0,-1};
        for(unsigned i = 0; i < 12; ++i) {
            val = hypotenuse(
                bx-(lattice_x->get(lx+x_list[i],ly+y_list[i])+x_list[i]*bs),
                by-(lattice_y->get(lx+x_list[i],ly+y_list[i])+y_list[i]*bs)
            );
            ans = (val < ans ? val : ans);
        }
    }
    return ans;
}

void ToroidalCellNoise::apply(ToroidalGrid* grid){
    if(grid->getSizeX() % this->getBlockSize() || grid->getSizeY() % this->getBlockSize())
        throw NoiseBlockException(this, grid);
    unsigned lattice_size_x = (grid->getSizeX()-1)/(this->getBlockSize())+1;
    unsigned lattice_size_y = (grid->getSizeY()-1)/(this->getBlockSize())+1;
    ToroidalGrid* lattice_x = new ToroidalGrid(lattice_size_x, lattice_size_y);
    ToroidalGrid* lattice_y = new ToroidalGrid(lattice_size_x, lattice_size_y);
    WhiteNoise lattice_noise(0.0,(double)(this->getBlockSize()));
    lattice_noise.setSeed(this->getSeed());
    lattice_noise.apply(lattice_x);
    lattice_noise.apply(lattice_y);
    for(unsigned i = 0; i < grid->getSizeX(); ++i)
    for(unsigned j = 0; j < grid->getSizeY(); ++j){
        grid->set(i,j,value(i,j,lattice_x,lattice_y));
    }
    grid->normalize(this->getMin(), this->getMax());
}

// PERLIN NOISE

inline double PerlinNoise::smoothstep(double x) {return (6*x*x-15*x+10)*x*x*x;}

void PerlinNoise::apply(Grid* grid){
    unsigned lattice_size_x = (grid->getSizeX()-1)/(this->getBlockSize())+2;
    unsigned lattice_size_y = (grid->getSizeY()-1)/(this->getBlockSize())+2;
    Grid lattice(lattice_size_x, lattice_size_y);
    WhiteNoise noise(0.0, 6.28);
    noise.setSeed(this->getSeed());
    noise.apply(&lattice);

    Grid* lattice_x = new Grid(&lattice);
    Grid* lattice_y = new Grid(&lattice);
    lattice_x->fun(&cos);
    lattice_y->fun(&sin);

    Grid* ul_grid = new Grid(grid->getSizeX(),grid->getSizeY());
    Grid* ll_grid = new Grid(grid->getSizeX(),grid->getSizeY());
    Grid* ur_grid = new Grid(grid->getSizeX(),grid->getSizeY());
    Grid* lr_grid = new Grid(grid->getSizeX(),grid->getSizeY());

    double bs = (double)(this->getBlockSize());
    for(unsigned ix = 0; ix < grid->getSizeX(); ix++) for(unsigned iy = 0; iy < grid->getSizeY(); iy++){
        unsigned lx = ix/(this->getBlockSize());
        unsigned ly = iy/(this->getBlockSize());
        double x = (double)(ix%(this->getBlockSize()))+0.5;
        double y = (double)(iy%(this->getBlockSize()))+0.5;

        ul_grid->set(ix,iy,lattice_x->get(lx,ly)*x + lattice_y->get(lx,ly)*y);
        ll_grid->set(ix,iy,lattice_x->get(lx+1,ly)*(x-bs) + lattice_y->get(lx+1,ly)*y);
        ur_grid->set(ix,iy,lattice_x->get(lx,ly+1)*x + lattice_y->get(lx,ly+1)*(y-bs));
        lr_grid->set(ix,iy,lattice_x->get(lx+1,ly+1)*(x-bs)+lattice_y->get(lx+1,ly+1)*(y-bs));
    }

    lerp(ul_grid,ur_grid,true);
    lerp(ll_grid,lr_grid,true);
    lerp(ul_grid,ll_grid,false);

    for(unsigned i = 0; i < grid->getSizeX(); ++i) for(unsigned j = 0; j < grid->getSizeY(); ++j){
        grid->set(i,j,ul_grid->get(i,j));
    }

    grid->normalize(this->getMin(), this->getMax());
}

void PerlinNoise::lerp(Grid* grid, Grid* grid_2, bool horizontal){
    unsigned i[2];
    unsigned h = (unsigned) horizontal;
    for(i[0] = 0; i[0] < (h ? grid->getSizeX() : grid->getSizeY()); ++i[0])
    for(i[1] = 0; i[1] < (h ? grid->getSizeY() : grid->getSizeX()); ++i[1]){
        int x = i[1] % (this->getBlockSize());
        grid->set(i[1-h],i[h],
        ((grid->get(i[1-h],i[h]))*(1-smoothstep((double)x/(double)(this->getBlockSize())))
        +(grid_2->get(i[1-h],i[h]))*smoothstep((double)x/(double)(this->getBlockSize())))
        /(this->getBlockSize()));
    }
}

void ToroidalPerlinNoise::apply(ToroidalGrid* grid){
    if(grid->getSizeX() % this->getBlockSize() || grid->getSizeY() % this->getBlockSize())
        throw NoiseBlockException(this, grid);
    unsigned lattice_size_x = (grid->getSizeX()-1)/(this->getBlockSize())+1;
    unsigned lattice_size_y = (grid->getSizeY()-1)/(this->getBlockSize())+1;
    Grid lattice(lattice_size_x, lattice_size_y);
    WhiteNoise noise(0.0, 6.28);
    noise.setSeed(this->getSeed());
    noise.apply(&lattice);

    ToroidalGrid* lattice_x = new ToroidalGrid(&lattice);
    ToroidalGrid* lattice_y = new ToroidalGrid(&lattice);
    lattice_x->fun(&cos);
    lattice_y->fun(&sin);

    Grid* ul_grid = new Grid(grid->getSizeX(),grid->getSizeY());
    Grid* ll_grid = new Grid(grid->getSizeX(),grid->getSizeY());
    Grid* ur_grid = new Grid(grid->getSizeX(),grid->getSizeY());
    Grid* lr_grid = new Grid(grid->getSizeX(),grid->getSizeY());

    double bs = (double)(this->getBlockSize());
    for(unsigned ix = 0; ix < grid->getSizeX(); ix++) for(unsigned iy = 0; iy < grid->getSizeY(); iy++){
        unsigned lx = ix/(this->getBlockSize());
        unsigned ly = iy/(this->getBlockSize());
        double x = (double)(ix%(this->getBlockSize()))+0.5;
        double y = (double)(iy%(this->getBlockSize()))+0.5;

        

        ul_grid->set(ix,iy,lattice_x->get(lx,ly)*x + lattice_y->get(lx,ly)*y);
        ll_grid->set(ix,iy,lattice_x->get(lx+1,ly)*(x-bs) + lattice_y->get(lx+1,ly)*y);
        ur_grid->set(ix,iy,lattice_x->get(lx,ly+1)*x + lattice_y->get(lx,ly+1)*(y-bs));
        lr_grid->set(ix,iy,lattice_x->get(lx+1,ly+1)*(x-bs)+lattice_y->get(lx+1,ly+1)*(y-bs));
    }

    lerp(ul_grid,ur_grid,true);
    lerp(ll_grid,lr_grid,true);
    lerp(ul_grid,ll_grid,false);

    for(unsigned i = 0; i < grid->getSizeX(); ++i) for(unsigned j = 0; j < grid->getSizeY(); ++j){
        grid->set(i,j,ul_grid->get(i,j));
    }

    grid->normalize(this->getMin(), this->getMax());
}

// FRACTAL NOISE

void FractalNoise::setNoise(BlockNoise* noise){
    this->noise = noise;
}

BlockNoise* FractalNoise::getNoise(){
    return this->noise;
}

void FractalNoise::setOctaves(unsigned octaves){
    this->octaves = octaves;
}

unsigned FractalNoise::getOctaves(){
    return this->octaves;
}

FractalNoise::FractalNoise(){}

FractalNoise::FractalNoise(double min, double max, BlockNoise* noise, unsigned octaves):
Noise(min, max), noise(noise), octaves(octaves){}

void FractalNoise::apply(Grid* grid){
    double weight = 1.0;
    unsigned block_size = this->noise->getBlockSize();
    unsigned org_block_size = block_size;
    for(unsigned i = 0; i < octaves; ++i){
        Grid lattice(grid->getSizeX(), grid->getSizeY());
        this->noise->apply(&lattice);
        lattice.mul(weight);
        grid->add(&lattice);
        weight /= 2;
        block_size /= 2;
        this->noise->setBlockSize(block_size);
    }
    this->noise->setBlockSize(org_block_size);
    grid->normalize(this->getMin(), this->getMax());
}
