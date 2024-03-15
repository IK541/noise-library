# Noise Library for C++

Following library allows for producing different noise types: white noise, value noise, cell noise (Worley noise), perlin noise and fractal noise.

Several examples are included in file ```examples.cpp```.

## Grid file

The grid file include ```Grid``` class and its subclass ```ToroidalGrid```, which represent
2D grids fo type double on which noises can be created. ```ToroidalGrid``` does not throw exception if indexing is out of bounds insteads it calculates index modulo size, therefore this type of grid is a topological torus.

Both classes have following attributes (which can be accessed through getters and setters):

```size_x``` - size in x dimension (note that x is vertical top down)

```size_y``` - size in y dimension (note that y is horizontal left to right)

```arr``` - a 2D grid of cells of type ```double```

Both classes have following methods:

```double get(unsigned x, unsigned y)``` - returns value of grid's cell at coordinates x, y (note that x is vertical top down and y horizontal left to right), will throw ```GridIndexException``` if index is out of bounds on non-toroidal grid

```set(unsigned x, unsigned y, double val)``` - sets value of cell x, y to val, will throw ```GridIndexException``` if index is out of bounds on non-toroidal grid

```set(unsigned x, unsigned y, Grid* grid)``` - overrides a part of this grid with values from another grid whose upper-left corner is placed on coordinates x, y, will throw ```GridIndexException``` if any cell of the applied grid is out of bounds on this grid and this grid is non-toroidal grid

```fun(double(*f)(double))``` - applies function f to each cell of the grid

```add(Grid* grid)``` - adds a value of corresponding cell in another gird to each, will throw ```GridSizeException``` if grids are of unequal size

```mul(double coef)``` - multiplies each cell of the grid by coef

```normalize(double min, double max)``` - linearly normalize values on the grid, so that the minimum value of cell is min and maximum value of cell is max, will throw ```GridNormalizationException``` if all value in the grid are equal

```fun(Grid* write_grid, Grid* read_grid, double(*f)(double,double))``` - a friend function, applies function f to each cell of write_grid, the second argument of f is value of the corresponding cell in read_grid, will throw ```GridSizeException``` if grids are of unequal size 

```write(int max, std::string file_name)``` - writes a grid to a file specified in file_name in .pgm format, values in the grid should be between 0 and max

```colorWrite(Grid* red, Grid* green, Grid* blue, int max, std::string file_name)``` - a friend function writes 3 grids to a file specified in file_name in .ppm format, each grid for different channel: R, G and B, values in each grid should be between 0 and max

Following getters and setters are available:

```setSizeX(unsigned size_x);``` - size_x setter

```unsigned getSizeX();``` - size_x getter

```setSizeY(unsigned size_y);``` - size_y setter

```unsigned getSizeY();``` - size_y getter

```setSize(unsigned size_x, unsigned size_y);``` - setter for both size_x and size_y

Following non-default constructors are available:

```Grid(unsigned size_x, unsigned size_y)``` - creates a grid of given size in x and y axes

```Grid(Grid* grid)``` - creates a deep copy of given grid

## Noise file

The noise file contains abstract class ```Noise``` with following subclasses:

```Noise``` (abstract)  
├── ```WhiteNoise```  
├── ```BlockNoise``` (abstract)  
│   ├── ```ValueNoise```  
│   │    └ ```ToroidalValueNoise```  
│   ├── ```CellNoise```  
│   │    └ ```ToroidalCellNoise```  
│   └── ```PerlinNoise```  
│        └ ```ToroidalPerlinNoise```  
└── ```FractalNoise```

Noise class possesses following attributes:

```seed``` - a seed for random generation of noise, two noises of the same type and seed applied to grids of the same size will be identical

```min``` - minimum value of a cell in generated noise

```max``` - maximum value of a cell in generated noise

Noise class has following methods:

```apply(Grid* grid)``` - applies a noise to given grid, result depends on noise subclass, all previous values of on the grid are overriden, will throw ```NoiseBlockException``` in case of toroidal block noises if size of the grid is not an integer multiple of the block size (this make occur during fractal noise generation if ti uses toroidal block noise)

Following getters and setters are available:

```setSeed(unsigned seed)``` - seed setter

```unsigned getSeed()``` - seed getter

```setMin(double min)``` - min setter

```double getMin();``` - min getter

```setMax(double max)``` - max setter

```double getMax()``` - max getter

```setMinMax(double min, double max)``` - setter for both min and max

A non-default contructor is available:

```Noise(double min, double max)``` - sets min and max to values given

Block noise has additional attribute and methods:

```block_size``` - size of block of generated noise, the lower the block size, the more detailed and similar to white noise the noise is

```setBlockSize(unsigned block_size)``` - block size setter

```unsigned getBlockSize()``` - block size getter

```BlockNoise(double min, double max, unsigned block_size)``` - constructor initializing min, max and block_size

Note that grid to which a toroidal noise is applied should be toroidal too.

Fractal noise has additional attributes and methods:

```noise``` - a block noise used for fractal noise generation

```octaves``` - number of octaves, for each octave block size is halved and the noise is added to the previous layer with half the weight

```setNoise(BlockNoise* noise)``` - noise setter

```BlockNoise* getNoise()``` - noise getter

```setOctaves(unsigned octaves)``` - octaves setter

```unsigned getOctaves()``` - octaves getter

```FractalNoise(double min, double max, BlockNoise* noise, unsigned octaves)``` - constructor initializing min, max, noise and octaves

## Exceptions file

Contains exceptions.

```GridNormalizationException``` - see ```normalize```

```GridIndexException``` - see ```set```, ```get```

```GridSizeException``` - see ```add```, ```fun (2)```

```NoiseBlockException``` - see ```apply```

## Compilation

Following commands can be used to make the library on Linux:

```g++ -c noise.cpp grid.cpp exceptions.cpp```

```ar r noise.a noise.o grid.o exceptions.o```

The resulting .a file will be used when compiling files using this library:

```g++ -o examples examples.cpp noise.a```

## Acknowledgent

I would like to thank Aeri4a for cooperating on this project before it was published on github.
