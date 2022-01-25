import { MemoryRouter as Router, Routes, Route } from 'react-router-dom';
import Data from './screens/Data';
import Devices from './screens/Devices';
import DeviceSettings from './screens/Device_settings';

import './App.css';
import { useSensors } from './hooks/useSensors';
import Sidebar from './components/Sidebar';

declare global {
  interface Window {
    electron: { ipcRenderer: Electron.IpcRenderer };
  }
}

export default function App() {
  const sensorController = useSensors();

  return (
    <Router>
      <Sidebar />
      <Routes>
        <Route path="/" element={<Devices {...sensorController} />} />
        <Route path="/data" element={<Data {...sensorController} />} />
        <Route
          path="/device_settings"
          element={<DeviceSettings {...sensorController} />}
        />
      </Routes>
    </Router>
  );
}
