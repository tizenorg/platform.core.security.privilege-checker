Name:    privilege-checker
Summary: Privilege Management
Version: 0.0.3
Release: 3
Group:   System/Libraries
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(libwbxml2)
BuildRequires: pkgconfig(libtzplatform-config)

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
Requires:	capi-security-privilege-manager

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
mkdir -p %{buildroot}%{TZ_SYS_DB}
cp util/res%{TZ_SYS_DB}/.privilegelist.db /%{buildroot}/%{TZ_SYS_DB}/

%make_install

%files -n privilege-checker
/usr/share/license/privilege-checker
%{TZ_SYS_BIN}/*
%{TZ_SYS_DB}/.privilegelist.db
%manifest packaging/privilege-checker.manifest

%files -n capi-security-privilege-manager
%{_libdir}/libcapi-security-privilege-manager.so*
/usr/share/locale/*
%manifest packaging/capi-security-privilege-manager.manifest

%files -n capi-security-privilege-manager-devel
%{_includedir}/privilegemgr/*.h

%clean
rm -rf %{buildroot}

