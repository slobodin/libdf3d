INCLUDE_DIRECTORIES(
 ./ConvexDecomposition
 ..
)

SET(ConvexDecomposition_SRCS
	bestfitobb.cpp
	ConvexBuilder.cpp
	cd_wavefront.cpp
	fitsphere.cpp
	meshvolume.cpp
	raytri.cpp
	vlookup.cpp
	bestfit.cpp
	cd_hull.cpp
	ConvexDecomposition.cpp
	concavity.cpp
	float_math.cpp
	planetri.cpp
	splitplane.cpp
)

SET(ConvexDecomposition_HDRS
	ConvexDecomposition.h
	cd_vector.h
	concavity.h
	bestfitobb.h
	ConvexBuilder.h
	cd_wavefront.h
	fitsphere.h
	meshvolume.h
	raytri.h
	vlookup.h
	bestfit.h
	cd_hull.h
)

ADD_LIBRARY(ConvexDecomposition ${ConvexDecomposition_SRCS} ${ConvexDecomposition_HDRS})

