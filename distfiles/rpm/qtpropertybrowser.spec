%global commit0 9568abd142d581b67b86a5f63d823a34b0612702
%global shortcommit0 %(c=%{commit0}; echo ${c:0:7})

Summary:    Qt Solutions Component: Property Browser
Name:       qtpropertybrowser
Version:    2.6.1.%{shortcommit0}git
Release:    1%{?dist}

License:    GPLv3 or LGPLv2 with exceptions
URL:        https://github.com/qtproject/qt-solutions/tree/master/qtpropertybrowser
Source0:    %{name}-%{commit0}.tar.gz
# Proposed upstream in https://codereview.qt-project.org/#/c/92417/
Source1:    qtpropertybrowser.prf

# Proposed upstream in https://codereview.qt-project.org/#/c/92411/
Source3:    LICENSE.GPL3
# Proposed upstream in https://codereview.qt-project.org/#/c/92411/
Source4:    LICENSE.LGPL
# Proposed upstream in https://codereview.qt-project.org/#/c/92411/
Source5:    LGPL_EXCEPTION

BuildRequires: qt4-devel 
BuildRequires: qt5-qtbase-devel 

%description
A property browser framework enabling the user to edit a set of
properties.

The framework provides a browser widget that displays the given
properties with labels and corresponding editing widgets (e.g.
line edits or comboboxes). The various types of editing widgets
are provided by the framework's editor factories: For each
property type, the framework provides a property manager (e.g.
QtIntPropertyManager and QtStringPropertyManager) which can be
associated with the preferred editor factory (e.g.
QtSpinBoxFactory and QtLineEditFactory). The framework also
provides a variant based property type with corresponding variant
manager and factory. Finally, the framework provides three
ready-made implementations of the browser widget:
QtTreePropertyBrowser, QtButtonPropertyBrowser and
QtGroupBoxPropertyBrowser.

%package devel
Summary:    Development files for %{name}
Requires:   %{name} = %{version}-%{release}
Requires:   qt4-devel

%description devel
This package contains libraries and header files for developing applications
that use qtpropertybrowser.

%package qt5
Summary:    Qt Solutions Component: Property Browser for Qt5
Requires:   qt5-qtbase

%description qt5
A property browser framework enabling the user to edit a set of
properties.

The framework provides a browser widget that displays the given
properties with labels and corresponding editing widgets (e.g.
line edits or comboboxes). The various types of editing widgets
are provided by the framework's editor factories: For each
property type, the framework provides a property manager (e.g.
QtIntPropertyManager and QtStringPropertyManager) which can be
associated with the preferred editor factory (e.g.
QtSpinBoxFactory and QtLineEditFactory). The framework also
provides a variant based property type with corresponding variant
manager and factory. Finally, the framework provides three
ready-made implementations of the browser widget:
QtTreePropertyBrowser, QtButtonPropertyBrowser and
QtGroupBoxPropertyBrowser.

This is a special build against Qt5.

%package qt5-devel
Summary:    Development files for %{name}-qt5
Requires:   %{name}-qt5 = %{version}-%{release}
Requires:   qt5-qtbase-devel

%description qt5-devel
This package contains libraries and header files for developing applications
that use qtpropertybrowser with Qt5.

%prep
%setup -n qt-solutions-%{commit0}/%{name} -q
# use versioned soname
sed -i "s,head,%(echo '%{version}' |sed -r 's,(.*)\..*,\1,'),g" common.pri
# do not build example source
sed -i /example/d %{name}.pro

mkdir licenses
cp -p %{SOURCE3} %{SOURCE4} %{SOURCE5} licenses

pushd ..
mkdir qt5
popd

cp -rf * ../qt5
cp -p %{SOURCE1} ../qt5
sed -i -r 's,-lQt,\05,' ../qt5/qtpropertybrowser.prf
sed -i -r 's,Target\(Qt,\05,' ../qt5/common.pri
mv ../qt5 .

%build
# Does not use GNU configure
./configure -library
%{qmake_qt4}
%make_build

pushd qt5
./configure -library
%{qmake_qt5}
%make_build
popd


%install
# libraries
mkdir -p %{buildroot}%{_libdir}
cp -a lib/* %{buildroot}%{_libdir}
cp -a qt5/lib/* %{buildroot}%{_libdir}
chmod 755 %{buildroot}%{_libdir}/*.so*

# headers
mkdir -p %{buildroot}%{_qt4_headerdir}/QtSolutions
cp -ap \
    src/qt*.h \
    src/Qt* \
    %{buildroot}%{_qt4_headerdir}/QtSolutions
mkdir -p %{buildroot}%{_qt5_headerdir}
# symlink is not possible due to split into individual subpackages
cp -ap %{buildroot}%{_qt4_headerdir}/QtSolutions %{buildroot}%{_qt5_headerdir}

mkdir -p %{buildroot}%{_qt4_datadir}/mkspecs/features %{buildroot}%{_qt5_archdatadir}/mkspecs/features
install -p -m644 %{SOURCE1} %{buildroot}%{_qt4_datadir}/mkspecs/features
install -p -m644 qt5/*.prf %{buildroot}%{_qt5_archdatadir}/mkspecs/features

%post -p /sbin/ldconfig    
%postun -p /sbin/ldconfig

%files
%license licenses/*
%doc README.TXT
# Caution! Unversioned .so file goes into -devel
%{_qt4_libdir}/libQtSolutions_PropertyBrowser*.so.*

%files devel
%doc doc/html/ examples/
%{_qt4_libdir}/libQtSolutions_PropertyBrowser*.so
%{_qt4_headerdir}/QtSolutions
%{_qt4_datadir}/mkspecs/features/qtpropertybrowser.prf

%post qt5 -p /sbin/ldconfig    
%postun qt5 -p /sbin/ldconfig

%files qt5
%license licenses/*
%doc README.TXT
# Caution! Unversioned .so file goes into -devel
%{_qt5_libdir}/libQt5*PropertyBrowser*.so.*

%files qt5-devel
%doc doc/html/ examples/
%{_qt5_libdir}/libQt5*PropertyBrowser*.so
%{_qt5_headerdir}/QtSolutions
%{_qt5_archdatadir}/mkspecs/features/qtpropertybrowser.prf

%changelog
* Thu Jul 25 2019 George 
- update for IR2 copr repo

* Sat Oct 7 2017 George
- initial version of spec file
