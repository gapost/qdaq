Summary: Qt based data aquisition
Name: qdaq
Version: v0.3.0
Release: 1%{?dist}
License: MIT
Source0: %{name}-%{version}.tar.gz

Requires: qt5-qtbase
Requires: qt5-qtbase-gui
Requires: qt5-qtscript
Requires: qt5-qtserialport
Requires: hdf5
Requires: gsl
Requires: muParser
Requires: qwt-qt5
Requires: libmodbus
Requires: linux-gpib
# Requires: comedilib
Requires: qtpropertybrowser-qt5

BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qtscript-devel
BuildRequires: qt5-qtserialport-devel
BuildRequires: qt5-qttools-static
BuildRequires: hdf5-devel
BuildRequires: gsl-devel
BuildRequires: muParser-devel
BuildRequires: qwt-qt5-devel
BuildRequires: libmodbus-devel
BuildRequires: linux-gpib-devel
# BuildRequires: comedilib-devel
BuildRequires: qtpropertybrowser-qt5-devel

%description
A Qt based framework for data aquisition applications.

%package devel
Summary:  Development files for %{name}
Requires: qwt-qt5-devel
Requires: %{name}%{?_isa} = %{version}-%{release}
%description devel
%{summary}.

%prep
%setup -q 

%build
mkdir %{_target_platform}
pushd %{_target_platform}
%{qmake_qt5} ..
make %{?_smp_mflags}
popd

%install
rm -rf %{buildroot}
make install INSTALL_ROOT=%{buildroot} -C %{_target_platform}

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
/usr/bin/qdaq
%{_qt5_libdir}/*.so.*
%{_qt5_plugindir}/*

%files devel
%{_qt5_headerdir}/QDaq/
%{_qt5_libdir}/*.so
%{_qt5_libdir}/qt5/mkspecs/features/*

%changelog
* Fri Sep 14 2020 George
- new v0.3.0
* Fri Sep 4 2020 George
- introduce devel package
- introduce Qt-feature prf file
* Fri Nov 15 2019 George
- ver 0.2.4
- fix bug in h5helper_v1_1
* Fri Jul 26 2019 George
- ver 0.2.3
- copr gapost/ir2
* Fri Feb 23 2018 George
- ver 0.2.0
- 1st release of v0.2
* Sat Oct 7 2017 George
- ver 0.1.1
- initial version of spec file
