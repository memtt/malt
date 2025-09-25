############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 07/2025
#    LICENSE  : CeCILL-C
#    FILE     : packaging/fedora/malt.spec
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015 - 2019
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

Name: malt
Version: 1.4.0-beta
Release: 1%{?dist}
Summary: Memory profiling tool to track memory allocations (malloc, realloc, free...)

Group: Development/Libraries
License: CeCILL-C
URL: https://github.com/downloads/svalat/svUnitTest/%{name}-%{version}.tar.bz2
Source0: %{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: cmake, gcc-c++ libunwind-devel elfutils-devel
Requires: libunwind elfutils-libelf binutils nodejs

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
