######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.1.0-dev                    #
#            DATE     : 02/2018                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

Name: malt
Version: 1.1.0
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
%{_bindir}/*
%{_libdir}/*
%{_datadir}/*
%{_includedir}/*
%doc

%changelog
