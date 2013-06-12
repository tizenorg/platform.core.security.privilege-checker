Name:    privilege-checker
Summary: Privilege Management
Version: 0.0.2
Release: 3
Group:   System/Libraries
License: SAMSUNG
Source0: %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(libwbxml2)

%description
Privilege Management

%package -n privilege-checker-devel
summary: privilege-checker server
Group: Development/Libraries
Requires: privilege-checker = %{version}-%{release}

%description -n privilege-checker-devel
privilege-checker devel

%package -n capi-security-privilege-manager
Summary:    Privilege Manager API
Group:      TO_BE/FILLED_IN
License:    TO BE FILLED IN
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(pkgmgr-info)

%description -n capi-security-privilege-manager
The Privilege Manager API provides functions to get information about privilege information of installed packages.

%package  -n capi-security-privilege-manager-devel
Summary:  Privilege Manager API (Development)
Group:    TO_BE/FILLED_IN

%description -n capi-security-privilege-manager-devel
The Privilege Manager API provides functions to get information about privilege information of installed packages.(DEV)

%prep
%setup -q

%build
#%{!?build_type:%define build_type "Release"}
echo cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%{version} \
        -DDPL_LOG="ON" 
cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%{version} \
        -DDPL_LOG="ON" 
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp LICENSE.APLv2 %{buildroot}/usr/share/license/privilege-checker
mkdir -p %{buildroot}/opt/dbspace
cp util/res/opt/dbspace/.privilegelist.db /%{buildroot}/opt/dbspace/

%make_install

%files -n privilege-checker
/usr/share/license/privilege-checker
/usr/bin/*
/opt/dbspace/.privilegelist.db
%manifest packaging/privilege-checker.manifest

%files -n capi-security-privilege-manager
%{_libdir}/libcapi-security-privilege-manager.so*
/usr/share/locale/*
%manifest packaging/capi-security-privilege-manager.manifest

%files -n capi-security-privilege-manager-devel
%{_includedir}/privilegemgr/*.h
%{_libdir}/libcapi-security-privilege-manager.so*
/usr/share/locale/*
%manifest packaging/capi-security-privilege-manager.manifest

%clean
rm -rf %{buildroot}

