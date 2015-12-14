Summary:        Remote dependency solving is a project which solves dependencies on server side/
Name:           rds
Version:        0.3.0
Release:        1%{?rpmrelease}%{?dist}
License:        GPLv2
Group:          Applications/File
URL:            https://github.com/rh-lab-q/server-side-dependency-solving

BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

Source0:        https://github.com/rh-lab-q/server-side-dependency-solving/archive/v{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  gcc
Requires:       libsolv

%description
Solving dependencies when installing new packages is a process
that in some cases can be computationally very intensive.
SSDS is designed to overcome this issue.
Client device gathers information about installed packages
and enabled repos which are then sent to a server.
The server then takes care of dependency solving
and the result is sent back to client device
as a list of packages that are needed to proceed
with the install process and without dependency solving on client.

%prep
%setup -q 

%build
%{cmake} 
make %{_smpflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR="%{buildroot}"

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%license LICENSE
%doc AUTHORS COPYING README
%doc %{_mandir}/man8/rear.8*

%changelog
* Mon 14 2015 Petr Hracek <phracek@redhat.com> - 0.3.0-1
- Initial release
