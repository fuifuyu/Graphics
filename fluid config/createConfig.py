from os import write


def write_coordinate(f,vec):
    f.write(str(len(vec)))
    f.write("\n")
    for v in vec:
        f.write(",".join(map(lambda x:str(x), v)))
        f.write("\n")

num_cells = 10
cell_size = 50
dt = 1
diffusion = 0.001
viscosity = 0.005
force = 500.
dens_source = 100.

f = open("config_small.csv","w")

f.write(",".join(map(lambda x: str(x), [num_cells,cell_size,dt,diffusion,viscosity,force,dens_source])))
f.write("\n")

mid = (int)((num_cells+2)/2)
# Velocity
veloc_x = []
veloc_y = []
mid = num_cells/2
for y in range(1,num_cells+1):
    for x in range(num_cells+1):
        veloc_x.append((x,y,-(mid-y)*5))
        veloc_y.append((x,y,(x-mid)*5))
write_coordinate(f,veloc_x)
write_coordinate(f,veloc_y)

# Density
min_range = (int)((num_cells+2)/2-2)
max_range = (int)((num_cells+2)/2+2)
dens = []
for i in range(min_range,max_range):
    for j in range(min_range,max_range):
        dens.append((i,j,200))
write_coordinate(f,dens)

f.close()