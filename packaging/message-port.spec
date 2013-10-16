
%define build_examples 1

Name: message-port
Summary: Message port daemon
Version: 0.0.1
Release: 1
Group: System/Daemons
License: LGPL-2.1+
Source: %{name}-%{version}.tar.gz

BuildRequires: pkgconfig(aul)
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
Group:      System/Libraries
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


%prep
%setup -q -n %{name}-%{version}
mkdir m4 > /dev/null
autoreconf -f -i


%build
%if %{build_examples} == 1
%configure --enable-debug --enable-examples
%else
%configure --enable-debug
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


# libmessage-port
%files -n lib%{name}
%defattr(-,root,root,-)
%doc AUTHORS COPYING.LIB README
%{_libdir}/lib%{name}.so*


#libmessage-port-devel
%files -n lib%{name}-devel
%defattr(-,root,root,-)
%if %{build_examples} == 1
%{_bindir}/msgport-example-app
%endif
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/*.h

