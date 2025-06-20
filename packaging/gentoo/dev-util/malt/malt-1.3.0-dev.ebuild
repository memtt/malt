############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 08/2015
#    LICENSE  : CeCILL-C
#    FILE     : packaging/gentoo/dev-util/malt/malt-1.3.1-dev.ebuild
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015
############################################################

EAPI=4

inherit cmake-utils

RESTRICT="primaryuri"
DESCRIPTION="Memory profiling tool to track memory allocations (malloc,free,realloc...)."
HOMEPAGE="https://github.com/svalat/malt"
SRC_URI="https://github.com/downloads/svalat/malt/malt-1.3.1.tar.bz2"

LICENSE="CeCILL-C"
SLOT="0"
KEYWORDS="~x86 ~amd64"
IUSE=""

DEPEND="sys-libs/libunwindd dev-libs/elfutils"
RDEPEND="sys-libs/libunwind dev-libs/elfutils net-libs/nodejs"

src_configure() {
	#local mycmakeargs=(-DXXX=YYY)
	cmake-utils_src_configure
}