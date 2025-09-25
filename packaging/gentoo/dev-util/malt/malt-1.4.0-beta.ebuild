############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 09/2025
#    LICENSE  : CeCILL-C
#    FILE     : packaging/gentoo/dev-util/malt/malt-1.4.0-beta.ebuild
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

EAPI=8

inherit cmake

DESCRIPTION="A memory allocation profiling tool for C/C++/Fortran."
HOMEPAGE="https://memtt.github.io/malt/"
SRC_URI="https://github.com/memtt/malt/releases/download/v${PV}/malt-${PV}.tar.bz2"
LICENSE="CeCILL-C"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="test +python +jemalloc"
RESTRICT=""
DEPEND="
	virtual/libelf
	sys-libs/libunwind
	dev-cpp/nlohmann_json
	dev-cpp/cpp-httplib
	dev-libs/openssl
	python? ( dev-lang/python )
	test? ( dev-cpp/gtest )
"
RDEPEND="
	${DEPEND}
	sys-devel/binutils
"

src_configure() {
	mycmakeargs=(
		-DENABLE_TESTS=$(usex test)
		-DENABLE_PYTHON=$(usex python)
		-DENABLE_JEMALLOC=$(usex jemalloc)
	)
	cmake_src_configure
}
