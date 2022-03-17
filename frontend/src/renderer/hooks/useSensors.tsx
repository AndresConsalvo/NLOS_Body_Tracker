import { useEffect, useState } from 'react';

type ServerEvent<T = unknown> = {
  type: string;
  data: T;
};

export type Sensor = {
  id: string;
  ip: string;
  accel: number[];
  gyro: number[];
  battery: string;
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
      [message.id]: message,
    }));
  };

  const changeRole = (sensor: Sensor, newRole: string) => {
    window.electron.ipcRenderer.sendToServer(
      JSON.stringify({
        type: 'CHANGE_ROLE',
        data: {
          id: sensor.id,
          body_part: newRole,
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
      console.log('arg', arg);
      const decoded = new TextDecoder().decode(arg);
      const payload: ServerEvent = JSON.parse(decoded);
      console.log('Starting python server...', payload);
      if (payload.type === 'DEVICE') {
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

export type SensorProps = ReturnType<typeof useSensors>;
