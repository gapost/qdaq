Summary: Qt based data aquisition
Name: qdaq
Version: v0.3.5
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


BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qtscript-devel
BuildRequires: qt5-qtserialport-devel
BuildRequires: qt5-qttools-static
BuildRequires: qthdf5
BuildRequires: gsl-devel
BuildRequires: muParser-devel
BuildRequires: qmatplotwidget
BuildRequires: qtpropertybrowser
BuildRequires: qconsolewidget
BuildRequires: libmodbus-devel
BuildRequires: linux-gpib-devel
# BuildRequires: comedilib-devel


%description
A Qt based framework for data acquisition applications.

%prep
%setup -q 

%build
%{cmake}
%{cmake_build}

%install
rm -rf %{buildroot}
%{cmake_install}

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_bindir}qdaq
%{_qt5_libdir}/*
%{_qt5_plugindir}/*


%changelog
* Tue Feb 10 2026 George
- cmake build
- v0.3.5
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
