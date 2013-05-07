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

%prep
%setup -q

%build
#%{!?build_type:%define build_type "Release"}
echo cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type}
cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp LICENSE.APLv2 %{buildroot}/usr/share/license/privilege-checker
mkdir -p %{buildroot}/opt/dbspace
cp res/opt/dbspace/.privilegelist.db /%{buildroot}/opt/dbspace/

%make_install

%files -n privilege-checker
/usr/share/license/privilege-checker
/usr/bin/*
/opt/dbspace/.privilegelist.db
%manifest packaging/privilege-checker.manifest

%clean
rm -rf %{buildroot}

