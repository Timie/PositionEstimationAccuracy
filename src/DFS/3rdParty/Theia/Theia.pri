INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/theia/math/polynomial.h \
    $$PWD/theia/sfm/pose/perspective_three_point.h \
    $$PWD/theia/sfm/pose/util.h \
    $$PWD/theia/math/find_polynomial_roots_companion_matrix.h \
    $$PWD/theia/alignment/alignment.h \
    $$PWD/theia/math/util.h \
    $$PWD/theia/matching/feature_correspondence.h \
    $$PWD/theia/sfm/feature.h \
    $$PWD/theia/util/random.h \
    $$PWD/theia/util/hash.h \
    $$PWD/theia/sfm/pose/dls_impl.h \
    $$PWD/theia/sfm/pose/dls_pnp.h

SOURCES += \
    $$PWD/theia/math/polynomial.cc \
    $$PWD/theia/sfm/pose/perspective_three_point.cc \
    $$PWD/theia/sfm/pose/util.cc \
    $$PWD/theia/math/find_polynomial_roots_companion_matrix.cc \
    $$PWD/theia/util/random.cc \
    $$PWD/theia/sfm/pose/dls_impl.cc \
    $$PWD/theia/sfm/pose/dls_pnp.cc
