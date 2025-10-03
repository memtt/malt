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
Version: 1.4.0
Release: 1%{?dist}
Summary: Memory profiling tool to track memory allocations (malloc, realloc, free...)

Group: Development/Libraries
License: CeCILL-C
URL: https://github.com/downloads/svalat/svUnitTest/%{name}-%{version}.tar.bz2
Source0: %{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: make cmake gcc-c++ libunwind-devel elfutils-devel openssl-devel graphviz nlohmann-json-devel cpp-httplib-devel python3-devel gtest
Requires: libunwind elfutils-libelf binutils nodejs openssl graphviz nlohmann-json-devel cpp-httplib python3

%description
Memory profiling tool to track memory allocations (malloc, realloc, free...)

%prep
%setup -q

%build
%cmake -DENABLE_TESTS=yes
%cmake_build

%install
%cmake_install

%clean
rm -rf $RPM_BUILD_ROOT

%check
%ctest

%files
%defattr(-,root,root,-)
%{_bindir}/malt
%{_bindir}/malt-webview
%{_bindir}/malt-passwd
%{_bindir}/malt-python
%{_libdir}/*
%{_datadir}/*
%{_includedir}/*
%doc

%changelog
