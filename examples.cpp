#include "noise.h"
#include <string>
#include <cmath>

int main() {
    // 5 types of noise are available

    Grid grid(256U,256U);

    WhiteNoise white_noise(0,255.0);
    white_noise.apply(&grid);
    grid.write(255,"1_white");

    ValueNoise value_noise(0.0,255.0,32U);
    value_noise.apply(&grid);
    grid.write(255,"2_value");

    CellNoise cell_noise(0.0,255.0,32U);
    cell_noise.apply(&grid);
    grid.write(255,"3_cell");

    PerlinNoise perlin_noise(0.0,255.0,32U);
    perlin_noise.apply(&grid);
    grid.write(255,"4_perlin");
    // if the same noise is applied to two different grids of the same size
    // the results are identical on both grids
    Grid grid_2(256U,256U);
    perlin_noise.apply(&grid_2);
    grid_2.write(255,"5_perlin_2");

    FractalNoise fractal_noise(0.0,255.0,&value_noise,4U);
    fractal_noise.apply(&grid);
    grid.write(255,"6_fractal");

    // when normal noise is used for tiling the borders between different grids are apparent

    Grid big_grid(512U,512U);
    perlin_noise.setBlockSize(64U);
    perlin_noise.apply(&grid);
    big_grid.set(0U,0U,&grid);
    big_grid.set(256U,0U,&grid);
    big_grid.set(0U,256U,&grid);
    big_grid.set(256U,256U,&grid);
    big_grid.write(255,"7_non_toroidal");

    // this is not the case with toroidal noise

    ToroidalPerlinNoise toroidal_perlin_noise(0.0,255.0,64U);
    ToroidalGrid toroidal_grid(256U,256U);
    toroidal_perlin_noise.apply(&toroidal_grid);
    big_grid.set(0U,0U,&toroidal_grid);
    big_grid.set(256U,0U,&toroidal_grid);
    big_grid.set(0U,256U,&toroidal_grid);
    big_grid.set(256U,256U,&toroidal_grid);
    big_grid.write(255,"8_toroidal");

    // a few examples

    cell_noise.setBlockSize(64U);
    fractal_noise.setNoise(&cell_noise);
    fractal_noise.apply(&grid);
    grid.fun([](double x){return x*x;});
    grid.normalize(0.0,255.0);
    grid_2.normalize(255.0*0.5,255.0*0.5);
    grid_2.add(&grid);
    grid_2.mul(0.667);
    colorWrite(&grid,&grid,&grid_2,255,"9_example_1");

    Grid big_grid_2(512U,512U);
    cell_noise.setBlockSize(128U);
    cell_noise.apply(&big_grid);
    big_grid.normalize(-4.0,16.0);
    big_grid.fun([](double x){return (exp(x-x*x)+exp(x))/(1+exp(x));});
    big_grid.normalize(0.0,96.0);
    fractal_noise.setNoise(&value_noise);
    fractal_noise.apply(&big_grid_2);
    big_grid.add(&big_grid_2);
    big_grid.normalize(0.0,255.0);
    big_grid.write(255,"10_example_2");

    big_grid.setSize(1024U,2048U);
    big_grid_2.setSize(1024U,2048U);
    perlin_noise.setMinMax(0.0,1.0);
    perlin_noise.setBlockSize(128U);
    perlin_noise.apply(&big_grid);
    big_grid.fun([](double x){return x > 0.75 ? ceil(16*(x-0.75)) : 0.0;});
    perlin_noise.setBlockSize(8U);
    perlin_noise.apply(&big_grid_2);
    big_grid_2.fun([](double x){return x > 0.8 ? 1.0 : 0.0;});
    fun(&big_grid,&big_grid_2,[](double x, double y){return x < 1.5 && y > 0.5 ? 5.1 : x+0.1;});
    Grid red_grid(1024U,2048U);
    Grid green_grid(1024U,2048U);
    Grid blue_grid(1024U,2048U);
    fun(&red_grid,&big_grid,[](double _, double x){
        double red_array[] = {0.0,0.0,247.0,111.0,0.0,255.0};
        return red_array[(unsigned)x];
    });
    fun(&green_grid,&big_grid,[](double _, double x){
        double green_array[] = {0.0,207.0,231.0,231.0,127.0,255.0};
        return green_array[(unsigned)x];
    });
    fun(&blue_grid,&big_grid,[](double _, double x){
        double blue_array[] = {127.0,255.0,175.0,111.0,0.0,255.0};
        return blue_array[(unsigned)x];
    });
    colorWrite(&red_grid,&green_grid,&blue_grid,255,"11_example_3");

    return 0;
}
