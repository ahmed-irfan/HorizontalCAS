# Advisory indices
COC=0
WL=1
WR=2
SL=3
SR=4

# Grid discretization
ACTIONS = [COC,WL,WR,SL,SR]
# AI: check in the training data -- these values are indices now, we should check what they mean in the training data
TAUS    = [0,1,5,10,20,40,60,80,100,101];

# maximum sensing region for sxu is 36656 (check the headers of the training data)
# we need to adapt the following values (XS and YS)
XS = convert(Array{Float32,1},vcat(LinRange(-5000,-3200,10),
          LinRange(-3000,-1100,20),
          LinRange(-1000,1000,41), LinRange(1100,3000,20),
          LinRange(3200,5000,10), LinRange(5250,10000,20),
          LinRange(10250,13000,12), LinRange(13500,15000,4)))
YS = convert(Array{Float32,1},vcat(LinRange(-10000,-5250,20),
          LinRange(-5000,-3200,10),
          LinRange(-3000,-1100,20),
          LinRange(-1000,1000,41), LinRange(1100,3000,20),
          LinRange(3200,5000,10), LinRange(5250,10000,20)))

PSIS_DEG = convert(Array{Float32,1},LinRange(-180.0,180.0,181))
PSIS = convert(Array{Float32,1},PSIS_DEG*pi/180.0)

NUMX = Int32(length(XS)-1)
NUMY = Int32(length(YS)-1)
NUMP = Int32(length(PSIS)-1)
NUMACTION= Int32(length(ACTIONS))
NUMTAU = Int32(length(TAUS))
NUMREGIONS=NUMX*NUMY*NUMP
