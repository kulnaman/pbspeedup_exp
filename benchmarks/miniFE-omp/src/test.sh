#!/bin/bash -xe

nx=100
ny=100
nz=100
echo "nx, ny, nz: $nx, $ny, $nz"
./miniFE.x -nx=${nx} -ny=${ny} -nz=${nz}

nx=200
echo "nx, ny, nz: $nx, $ny, $nz"
./miniFE.x -nx=${nx} -ny=${ny} -nz=${nz}

ny=200
echo "nx, ny, nz: $nx, $ny, $nz"
./miniFE.x -nx=${nx} -ny=${ny} -nz=${nz}


nx=400
echo "nx, ny, nz: $nx, $ny, $nz"
./miniFE.x -nx=${nx} -ny=${ny} -nz=${nz}

ny=400
echo "nx, ny, nz: $nx, $ny, $nz"
./miniFE.x -nx=${nx} -ny=${ny} -nz=${nz}
