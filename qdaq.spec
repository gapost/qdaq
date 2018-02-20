Summary: Qt based data aquisition
Name: qdaq
Version: 0.1.1
Release: 1%{?dist}
License: MIT
Source0: %{name}.tar.gz

Requires: qt5-qtbase
Requires: qt5-qtbase-gui
Requires: qt5-qtscript
Requires: qt5-qtserialport
Requires: hdf5
Requires: gsl
Requires: muParser
Requires: qwt-qt5
Requires: libmodbus

# from maxiotis:ir2
Requires: libgpib0 
Requires: comedilib
Requires: libQtSolutions_PropertyBrowser1

BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qtscript-devel
BuildRequires: qt5-qtserialport-devel
BuildRequires: qt5-qttools-static
BuildRequires: hdf5-devel
BuildRequires: gsl-devel
BuildRequires: muParser-devel
BuildRequires: qwt-qt5-devel
BuildRequires: libmodbus-devel

# from maxiotis:ir2
BuildRequires: gpib-devel
BuildRequires: comedilib-devel
BuildRequires: libQtSolutions_PropertyBrowser1-devel

%description
A Qt based framework for data aquisition applications.

%prep
#%setup -q -b 1
%setup -q -n %{name}
#cd lib
#tar -zxvf %{SOURCE1}

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

%post

%postun

%files
%defattr(-,root,root)
/usr/bin/qdaq
/usr/include/*
/usr/%{_lib}/*

%changelog
* Sat Oct 7 2017 George
- ver 0.1.1
- initial version of spec file
