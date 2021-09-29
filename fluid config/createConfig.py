from os import write


def write_coordinate(f,vec):
    f.write(str(len(vec)))
    f.write("\n")
    for v in vec:
        f.write(",".join(map(lambda x:str(x), v)))
        f.write("\n")

num_cells = 100
cell_size = 5
dt = 0.01
diffusion = 0.001
viscosity = 0.005
force = 500.
dens_source = 100.

f = open("config.csv","w")

f.write(",".join(map(lambda x: str(x), [num_cells,cell_size,dt,diffusion,viscosity,force,dens_source])))
f.write("\n")

mid = (int)((num_cells+2)/2)
# X Component
veloc = []
veloc.append((mid-10,mid,force/cell_size))
veloc.append((mid+10,mid,-force/cell_size))
write_coordinate(f,veloc)

# Y Component
veloc = []
write_coordinate(f,veloc)

# Density
min_range = (int)((num_cells+2)/2-10)
max_range = (int)((num_cells+2)/2+10)
dens = []
for i in range(min_range,max_range):
    for j in range(min_range,max_range):
        dens.append((i,j,200))
write_coordinate(f,dens)

f.close()