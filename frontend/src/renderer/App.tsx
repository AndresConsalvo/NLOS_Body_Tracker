import { MemoryRouter as Router, Routes, Route } from 'react-router-dom';
import Data from './screens/Data';
import Devices from './screens/Devices';
import DeviceSettings from './screens/Device_settings';

import './App.css';
import Sidebar from './components/Sidebar';
import { AppProvider } from './hooks/useAppContext';

declare global {
  interface Window {
    electron: { ipcRenderer: Electron.IpcRenderer };
  }
}

export default function App() {
  return (
    <AppProvider>
      <Router>
        <Sidebar />
        <Routes>
          <Route path="/" element={<Devices />} />
          <Route path="/data" element={<Data />} />
          <Route path="/device_settings" element={<DeviceSettings />} />
        </Routes>
      </Router>
    </AppProvider>
  );
}
