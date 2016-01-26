Name:    privilege-checker
Summary: Privilege Management
Version: 0.0.5
Release: 3
Group:   System/Libraries
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(sqlite3)
BuildRequires:  pkgconfig(glib-2.0)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Pakcage for Privilege Management

%package -n privilege-checker-devel
summary: privilege-checker-devel
Group: Development/Libraries
Requires: privilege-checker = %{version}-%{release}

%description -n privilege-checker-devel
Package for Privilege Management (DEV)

%package -n security-privilege-manager
Summary:    Privilege Manager API
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(glib-2.0)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n security-privilege-manager
The Privilege Manager API provides functions to verify privilege information of packages to be installed.

%package  -n security-privilege-manager-devel
Summary:  Privilege Manager API (Development)
Requires: security-privilege-manager

%description -n security-privilege-manager-devel
The Privilege Manager API provides functions to verify privilege information of packages to be installed.(DEV)

%package  -n tc-privilege-checker
Summary:  tc-privilege-checker

%description -n tc-privilege-checker
Testsuit for Privilege Manager APIs

%prep
%setup -q

%build

%if "%{?profile}" == "tv"
    __PROFILE_TYPE="TV"
%else
%if "%{?profile}" == "wearable"
    __PROFILE_TYPE="WEARABLE"
%else
%if "%{?profile}" == "mobile"
    __PROFILE_TYPE="MOBILE"
%endif
%endif
%endif

export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"

echo cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%{version} \
        -DDPL_LOG="ON" \
        -DDATADIR=%{_datadir} \
        -DPROFILE_TYPE="${__PROFILE_TYPE}"

cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%{version} \
        -DDPL_LOG="ON" \
        -DDATADIR=%{_datadir} \
        -DPROFILE_TYPE="${__PROFILE_TYPE}"

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_datadir}/license
cp LICENSE.APLv2 %{buildroot}%{_datadir}/license/privilege-checker
mkdir -p %{buildroot}/opt/dbspace
mkdir -p %{buildroot}%{_datadir}/privilege-manager
#%if "%{?profile}" == "tv"
#cp capi/res/dbspace/tv_wrt_privilege_info.db %{buildroot}%{_datadir}/privilege-manager/.wrt_privilege_info.db
#sqlite3 /%{buildroot}%{_datadir}/privilege-manager/.wrt_privilege_info.db "select * from privilege_info"
#cp capi/res/dbspace/tv_core_privilege_info.db %{buildroot}%{_datadir}/privilege-manager/.core_privilege_info.db
#sqlite3 /%{buildroot}%{_datadir}/privilege-manager/.core_privilege_info.db "select * from privilege_info"
#cp capi/res/dbspace/tv_wrt_privilege_mapping.db %{buildroot}%{_datadir}/privilege-manager/.wrt_privilege_mapping.db
#sqlite3 /%{buildroot}%{_datadir}/privilege-manager/.wrt_privilege_mapping.db "select * from privilege_mapping"
#cp capi/res/dbspace/tv_core_privilege_mapping.db %{buildroot}%{_datadir}/privilege-manager/.core_privilege_mapping.db
#sqlite3 /%{buildroot}%{_datadir}/privilege-manager/.core_privilege_mapping.db "select * from privilege_mapping"
#%else
#cp capi/res/dbspace/core_privilege_info.db %{buildroot}%{_datadir}/privilege-manager/.core_privilege_info.db
#sqlite3 /%{buildroot}%{_datadir}/privilege-manager/.core_privilege_info.db "select * from privilege_info"
#cp capi/res/dbspace/wrt_privilege_info.db %{buildroot}%{_datadir}/privilege-manager/.wrt_privilege_info.db
#sqlite3 /%{buildroot}%{_datadir}/privilege-manager/.wrt_privilege_info.db "select * from privilege_info"
#cp capi/res/dbspace/core_privilege_mapping.db %{buildroot}%{_datadir}/privilege-manager/.core_privilege_mapping.db
#sqlite3 /%{buildroot}%{_datadir}/privilege-manager/.core_privilege_mapping.db "select * from privilege_mapping"
#cp capi/res/dbspace/wrt_privilege_mapping.db %{buildroot}%{_datadir}/privilege-manager/.wrt_privilege_mapping.db
#sqlite3 /%{buildroot}%{_datadir}/privilege-manager/.wrt_privilege_mapping.db "select * from privilege_mapping"
#%endif
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%post -n security-privilege-manager -p /sbin/ldconfig
%postun -n security-privilege-manager -p /sbin/ldconfig

%files -n privilege-checker
%{_datadir}/license/privilege-checker
%manifest packaging/privilege-checker.manifest

%files -n security-privilege-manager
%{_libdir}/libsecurity-privilege-manager.so*
%{_datadir}/locale/*
#%if "%{?profile}" != "tv"
%{_datadir}/privilege-manager/.core_privilege_info.db
%{_datadir}/privilege-manager/.core_privilege_mapping.db
#%endif
%{_datadir}/privilege-manager/.wrt_privilege_info.db
%{_datadir}/privilege-manager/.wrt_privilege_mapping.db
%manifest packaging/security-privilege-manager.manifest

%files -n security-privilege-manager-devel
%{_includedir}/privilegemgr/*.h
%{_libdir}/pkgconfig/security-privilege-manager.pc

%files -n tc-privilege-checker
%{_bindir}/tc-privilege-db-manager
%{_bindir}/tc-privilege-manager
%{_bindir}/tc-privilege-info

%clean
rm -rf %{buildroot}

