
%define build_tests 1
%define use_session_bus 1
Name: message-port
Summary: Message port daemon
Version: 0.0.1
Release: 1
Group: System/Service
License: LGPL-2.1+
Source: %{name}-%{version}.tar.gz

BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(gio-2.0)
BuildRequires: pkgconfig(gio-unix-2.0)
BuildRequires: pkgconfig(glib-2.0) >= 2.30
BuildRequires: pkgconfig(gobject-2.0)
BuildRequires: pkgconfig(pkgmgr-info)

%description
This daemon allows the webapplications to communicates using 
Tizen MessagePort WebAPI.


%package -n lib%{name}
Summary:    Client library for message port
Group:      Base/Libraries
Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires: %{name} = %{version}-%{release} 
BuildRequires: pkgconfig(bundle)

%description -n lib%{name}
Client library that porvies C APIs to work with message port.


%package -n lib%{name}-devel
Summary:    Development files for libmessage-port 
Group:      Development/Libraries
Requires:   lib%{name} = %{version}-%{release}

%description -n lib%{name}-devel
Development files for message-port client library.

%if %{build_tests} == 1

%package -n %{name}-tests
Summary: Unit tests for messageport
Group: Development/Testing
Requires: lib%{name} = %{version}-%{release}

%description -n %{name}-tests
Unit tests for messageport implementation.

%endif


%prep
%setup -q -n %{name}-%{version}
mkdir m4 > /dev/null
autoreconf -f -i


%build
%configure \
%if %{build_tests} == 1
     --enable-tests\
%endif
%if %{use_session_bus} == 1
    --enable-sessionbus \
%endif

make %{?_smp_mflags}


%install
%make_install


%post -n lib%{name}
/sbin/ldconfig

%postun -n lib%{name}
/sbin/ldconfig


# daemon: message-port
%files -n %{name}
%defattr(-,root,root,-)
%{_bindir}/messageportd
%if %{use_session_bus} == 1
%{_datadir}/dbus-1/services/org.tizen.messageport.service
%endif


# libmessage-port
%files -n lib%{name}
%defattr(-,root,root,-)
%doc AUTHORS COPYING.LIB README
%{_libdir}/lib%{name}.so*


#libmessage-port-devel
%files -n lib%{name}-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/*.h

%if %{build_tests} == 1
%files -n %{name}-tests
%{_bindir}/msgport-test-app
%{_bindir}/msgport-test-app-cpp
%endif
