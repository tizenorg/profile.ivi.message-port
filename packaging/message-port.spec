Name: message-port
Summary; Message port support for webapps communication
Version: 0.0.1
Release: 1
Group: System/Libraries
License: LGPL-2.1+
Source: %{name}-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires: pkgconfig(gio-2.0)
BuildRequires: pkgconfig(gio-unix-2.0)
BuildRequires: pkgconfig(glib-2.0) >= 2.30
BuildRequires: pkgconfig(gobject-2.0)

%description
%{summery}.

%package -n messageportd
Summary; Message port daemon
Group:System/Daemons
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(pkgmgr-info)

%description -n messageportd
%{summery}.

%package -n lib%{name%}
Summary: Client library for message port
Group: System/Libraries
BuildRequires: pkgconfig(bundle)

%description -n lib%{name}
%{summery}.

%package -n lib%{name}-devel
Summary:    Development files for message port client library
Group:      Development/Libraries
Requires:   lib%{name} = %{version}-%{release}

%description -n lib%{name}-devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}
autoreconf -f -i

%build
%configure --enablue-debug
make %{?_smp_mflags}

%install
%make_install

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%files -n messageportd
%defattr(-,root,root,-)
%doc AUTHORS COPYING.LIB INSTALL NEWS README
%{_bindir}/messageportd

%files -n lib%{name}
%defattr(-,root,root,-)
%{_libdir}/lib%{name}.so.*

%files -n lib%{name}-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/lib%{name}.pc
%{_includedir}/*.h
