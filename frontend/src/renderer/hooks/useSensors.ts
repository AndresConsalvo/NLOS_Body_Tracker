import { useEffect, useState } from 'react';

type ServerEvent<T = unknown> = {
  type: string;
  data: T;
};

export type Sensor = {
  ip: string;
  //position: number[];
  accel: number[];
  gyro: number[];
  battery: string;
  // battery_V: number;
  id: string;
  // IMU_accuracy: string;
  body_part: string;
};

export type SensorMap = {
  [key: string]: Sensor;
};

export const useSensors = () => {
  const [devices, setDevices] = useState<SensorMap>({});

  const handleMessage = (message: Sensor) => {
    setDevices((currentDevices) => ({
      ...currentDevices,
      [message.ip]: message,
    }));
  };

  const changeRole = (sensor: Sensor, newRole: string) => {
    window.electron.ipcRenderer.sendToServer(
      JSON.stringify({
        type: 'CHANGE_ROLL',
        data: {
          ip: sensor.ip,
          role: newRole,
        },
      })
    );
  };

  // Adds new position to exisiting de
  // const handleNewPosition = (ip: string, newPositions: number[]) => {
  //   setDevices((currentDevices) => ({
  //     ...currentDevices,
  //     [ip]: {
  //       ...currentDevices[ip],
  //       position: newPositions,
  //     },
  //   }));
  // };

  const init = () => {
    window.electron.ipcRenderer.on('ipc-python', (arg) => {
      const decoded = new TextDecoder().decode(arg);
      const payload: ServerEvent = JSON.parse(decoded);
      console.log('AQUI', payload);
      if (payload.type === 'DEVICE_STATS') {
        handleMessage(payload.data as ServerEvent<Sensor>['data']);
      }
    });
    window.electron.ipcRenderer.python();
  };

  useEffect(() => {
    init();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  const addDevice = (deviceName: string, ip: string) => {
    // Do stuff
  };

  return { devices, addDevice, changeRole };
};
