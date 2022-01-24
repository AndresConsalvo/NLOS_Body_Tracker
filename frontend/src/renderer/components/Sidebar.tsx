import { Link, useLocation } from 'react-router-dom';

const routes = [
  { path: '/', label: 'Devices' },
  { path: '/device_settings', label: 'Device_Settings' },
  { path: '/data', label: 'Data' },
];

export default () => {
  const { pathname } = useLocation();
  return (
    <div className="sidenav">
      {routes.map((route) => (
        <Link
          className={pathname === route.path ? 'active' : ''}
          to={route.path}
        >
          {route.label}
        </Link>
      ))}
    </div>
  );
};
