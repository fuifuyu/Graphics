from os import write


def write_coordinate(f,vec):
    f.write(str(len(vec)))
    f.write("\n")
    for v in vec:
        f.write(",".join(map(lambda x:str(x), v)))
        f.write("\n")

N = 100
h = 5
dt = 0.01
diff = 0.001
visc = 0.005
force = 500.
source = 100.

f = open("config.csv","w")

f.write(",".join(map(lambda x: str(x), [N,h,dt,diff,visc,force,source])))
f.write("\n")

mid = (int)((N+2)/2)
# X Component
veloc = []
veloc.append((mid-10,mid,force/h))
veloc.append((mid+10,mid,-force/h))
write_coordinate(f,veloc)

# Y Component
veloc = []
write_coordinate(f,veloc)

# Density
min_range = (int)((N+2)/2-10)
max_range = (int)((N+2)/2+10)
dens = []
for i in range(min_range,max_range):
    for j in range(min_range,max_range):
        dens.append((i,j,200))
write_coordinate(f,dens)

f.close()