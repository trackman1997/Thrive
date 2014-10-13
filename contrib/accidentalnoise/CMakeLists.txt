cmake_minimum_required(VERSION 2.8)

project(accidentalnoise CXX)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include})
set(LUA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lua})

set(LIB_SOURCES
    ${SOURCE_DIR}/arrayoperations.cpp
    ${SOURCE_DIR}/cellulargen.cpp
    ${SOURCE_DIR}/hsv.cpp
    ${SOURCE_DIR}/implicitautocorrect.cpp
    ${SOURCE_DIR}/implicitbasisfunction.cpp
    ${SOURCE_DIR}/implicitbias.cpp
    ${SOURCE_DIR}/implicitblend.cpp
    ${SOURCE_DIR}/implicitbrightcontrast.cpp
    ${SOURCE_DIR}/implicitcache.cpp
    ${SOURCE_DIR}/implicitcellular.cpp
    ${SOURCE_DIR}/implicitclamp.cpp
    ${SOURCE_DIR}/implicitcombiner.cpp
    ${SOURCE_DIR}/implicitconstant.cpp
    ${SOURCE_DIR}/implicitcos.cpp
    ${SOURCE_DIR}/implicitcurve.cpp
    ${SOURCE_DIR}/implicitextractrgbachannel.cpp
    ${SOURCE_DIR}/implicitfloor.cpp
    ${SOURCE_DIR}/implicitfractal.cpp
    ${SOURCE_DIR}/implicitfunctiongradient.cpp
    ${SOURCE_DIR}/implicitgain.cpp
    ${SOURCE_DIR}/implicitgradient.cpp
    ${SOURCE_DIR}/implicitmagnitude.cpp
    ${SOURCE_DIR}/implicitmodulebase.cpp
    ${SOURCE_DIR}/implicitnormalizecoords.cpp
    ${SOURCE_DIR}/implicitpow.cpp
    ${SOURCE_DIR}/implicitrgbadotproduct.cpp
    ${SOURCE_DIR}/implicitrotatedomain.cpp
    ${SOURCE_DIR}/implicitsawtooth.cpp
    ${SOURCE_DIR}/implicitscaledomain.cpp
    ${SOURCE_DIR}/implicitscaleoffset.cpp
    ${SOURCE_DIR}/implicitselect.cpp
    ${SOURCE_DIR}/implicitsin.cpp
    ${SOURCE_DIR}/implicitsphere.cpp
    ${SOURCE_DIR}/implicittiers.cpp
    ${SOURCE_DIR}/implicittranslatedomain.cpp
    ${SOURCE_DIR}/implicittriangle.cpp
    ${SOURCE_DIR}/mapping.cpp
    ${SOURCE_DIR}/noise_gen.cpp
    ${SOURCE_DIR}/noise_lut.cpp
    ${SOURCE_DIR}/rasterization.cpp
    ${SOURCE_DIR}/rgbablend.cpp
    ${SOURCE_DIR}/rgbablendops.cpp
    ${SOURCE_DIR}/rgbacolorops.cpp
    ${SOURCE_DIR}/rgbacomposechannels.cpp
    ${SOURCE_DIR}/rgbaconstant.cpp
    ${SOURCE_DIR}/rgbacurve.cpp
    ${SOURCE_DIR}/rgbahsvtorgba.cpp
    ${SOURCE_DIR}/rgbaimplicitgrayscale.cpp
    ${SOURCE_DIR}/rgbanormalize.cpp
    ${SOURCE_DIR}/rgbargbatohsv.cpp
    ${SOURCE_DIR}/rgbarotatecolor.cpp
    ${SOURCE_DIR}/rgbaselect.cpp
)

set(LIB_HEADERS
    ${SOURCE_DIR}/anl.h
    ${SOURCE_DIR}/arrayoperations.h
    ${SOURCE_DIR}/cellulargen.h
    ${SOURCE_DIR}/hsv.h
    ${SOURCE_DIR}/implicitautocorrect.h
    ${SOURCE_DIR}/implicitbasisfunction.h
    ${SOURCE_DIR}/implicitbias.h
    ${SOURCE_DIR}/implicitblend.h
    ${SOURCE_DIR}/implicitbrightcontrast.h
    ${SOURCE_DIR}/implicitcache.h
    ${SOURCE_DIR}/implicitcellular.h
    ${SOURCE_DIR}/implicitclamp.h
    ${SOURCE_DIR}/implicitcombiner.h
    ${SOURCE_DIR}/implicitconstant.h
    ${SOURCE_DIR}/implicitcos.h
    ${SOURCE_DIR}/implicitcurve.h
    ${SOURCE_DIR}/implicitextractrgbachannel.h
    ${SOURCE_DIR}/implicitfloor.h
    ${SOURCE_DIR}/implicitfractal.h
    ${SOURCE_DIR}/implicitfunctiongradient.h
    ${SOURCE_DIR}/implicitgain.h
    ${SOURCE_DIR}/implicitgradient.h
    ${SOURCE_DIR}/implicitmagnitude.h
    ${SOURCE_DIR}/implicitmodifier.h
    ${SOURCE_DIR}/implicitmodulebase.h
    ${SOURCE_DIR}/implicitnormalizecoords.h
    ${SOURCE_DIR}/implicitpow.h
    ${SOURCE_DIR}/implicitrgbadotproduct.h
    ${SOURCE_DIR}/implicitrotatedomain.h
    ${SOURCE_DIR}/implicitsawtooth.h
    ${SOURCE_DIR}/implicitscaledomain.h
    ${SOURCE_DIR}/implicitscaleoffset.h
    ${SOURCE_DIR}/implicitselect.h
    ${SOURCE_DIR}/implicitsin.h
    ${SOURCE_DIR}/implicitsphere.h
    ${SOURCE_DIR}/implicittiers.h
    ${SOURCE_DIR}/implicittransform.h
    ${SOURCE_DIR}/implicittranslatedomain.h
    ${SOURCE_DIR}/implicittriangle.h
    ${SOURCE_DIR}/mapping.h
    ${SOURCE_DIR}/noise_gen.h
    ${SOURCE_DIR}/noise_lut.h
    ${SOURCE_DIR}/noiselut.h
    ${SOURCE_DIR}/random_gen.h
    ${SOURCE_DIR}/rasterization.h
    ${SOURCE_DIR}/rgbablend.h
    ${SOURCE_DIR}/rgbablendops.h
    ${SOURCE_DIR}/rgbacolorops.h
    ${SOURCE_DIR}/rgbacomposechannels.h
    ${SOURCE_DIR}/rgbaconstant.h
    ${SOURCE_DIR}/rgbacurve.h
    ${SOURCE_DIR}/rgba.h
    ${SOURCE_DIR}/rgbahsvtorgba.h
    ${SOURCE_DIR}/rgbaimplicitgrayscale.h
    ${SOURCE_DIR}/rgbamodulebase.h
    ${SOURCE_DIR}/rgbanormalize.h
    ${SOURCE_DIR}/rgbargbatohsv.h
    ${SOURCE_DIR}/rgbarotatecolor.h
    ${SOURCE_DIR}/rgbaselect.h
    ${SOURCE_DIR}/utility.h
    ${SOURCE_DIR}/vectortypes.h
    ${SOURCE_DIR}/templates/tarrays.h
    ${SOURCE_DIR}/templates/tcurve.h
    ${SOURCE_DIR}/templates/tvec2d.h
    ${SOURCE_DIR}/templates/tvec3d.h
    ${SOURCE_DIR}/templates/tvec4d.h
)

set_source_files_properties(
    ${LIB_SOURCES}
    PROPERTIES LANGUAGE CXX
)

include_directories(
    ${SOURCE_DIR}/
    ${SOURCE_DIR}/template/
)

add_library(accidentalnoise SHARED
    ${LIB_SOURCES}
    ${LIB_HEADERS}
)

install(TARGETS accidentalnoise
    EXPORT accidentalnoise
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)
