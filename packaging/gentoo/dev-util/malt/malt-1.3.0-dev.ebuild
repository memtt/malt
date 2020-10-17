######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

EAPI=4

inherit cmake-utils

RESTRICT="primaryuri"
DESCRIPTION="Memory profiling tool to track memory allocations (malloc,free,realloc...)."
HOMEPAGE="https://github.com/svalat/malt"
SRC_URI="https://github.com/downloads/svalat/malt/malt-1.2.2.tar.bz2"

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