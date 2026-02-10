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
BuildRequires: hdf5-devel
BuildRequires: qthdf5
BuildRequires: gsl-devel
BuildRequires: muParser-devel
BuildRequires: qwt-qt5-devel
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
%{_bindir}/qdaq
%{_qt5_libdir}/*
%{_qt5_plugindir}/*


%changelog
* Tue Feb 10 2026 George
- cmake build
- v0.3.5
* Mon Dec 22 2025 George
- ver 0.3.4
* Mon Jul 1 2024 George
- rebuild for EL9
- comment-out comedilib
* Fri Feb 16 2024 George
- added /usr/bin/qdaq as link to exec
* Mon Feb 5 2024 George
- ver 0.3.3
- added submodules qthdf5, qconsolewidget 
* Fri Aug 27 2021 George
- ver 0.3.1
- minor bugfixes
* Wed Jul 7 2021 George
- ver 0.3.0
- major upgrade
* Tue Jun 30 2020 George
- ammended ver 0.2.7
* Fri Sep 14 2020 George
- new v0.3.0
* Fri Sep 4 2020 George
- introduce devel package
- introduce Qt-feature prf file
* Fri May 15 2020 George
- ver 0.2.6
- MATLAB-like plot attributes in QDaqPlot (panos)
- Dynamically add/remove channels in QDaqDataBuffer (panos)
* Thu Feb 13 2020 George
- ver 0.2.5
- implemented median filter (panos)
- reorganized QDaq types and code cleaning (panos)
- work on windows build - plugin & dll placement, gpib support (panos)
- corrected behavior in loopEngine(), QDaqInterpolator, QDaqDataBuffer serialization to HDF5
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

