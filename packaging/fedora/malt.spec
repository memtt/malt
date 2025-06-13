############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 09/2019
#    LICENSE  : CeCILL-C
#    FILE     : packaging/fedora/malt.spec
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015 - 2019
#    AUTHOR   : Tru Huynh - 2018
############################################################

Name: malt
Version: 1.3.0
Release: 1%{?dist}
Summary: Memory profiling tool to track memory allocations (malloc, realloc, free...)

Group: Development/Libraries
License: CeCILL-C
URL: https://github.com/downloads/svalat/svUnitTest/%{name}-%{version}.tar.bz2
Source0: %{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: cmake, gcc-c++ libunwind-devel elfutils-devel qt5-qtwebkit-devel
Requires: libunwind elfutils-libelf binutils nodejs qt5-qtwebkit graphviz

%description
Memory profiling tool to track memory allocations (malloc, realloc, free...)

%prep
%setup -q

%build
%cmake
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_bindir}/malt
%{_bindir}/malt-webview
%{_bindir}/malt-passwd
%{_libdir}/*
%{_datadir}/*
%{_includedir}/*
%doc

%changelog

#sub package for QT5
%package qt5
Requires: malt qt5-qtwebkit
Summary: Provide the QT5 based view if you do not want to run your browser.

%description qt5
Provide the QT5 based view if you do not want to run your browser.

%files qt5
%defattr(-,root,root,-)
%{_bindir}/malt-qt5
%doc

