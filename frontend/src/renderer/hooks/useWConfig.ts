import React, { useEffect, useState } from 'react';

type ServerEvent<T = unknown> = {
  type: string;
  data: T;
};

const initialState = {
  COM: {
    label: 'COM Port',
    value: '',
  },
  SSID: {
    label: 'SSID',
    value: '',
  },
  Password: {
    label: 'Password',
    value: '',
  },
};

export const useWConfig = () => {
  const [config, setConfig] = useState(initialState);
  const handleWIFI: React.ChangeEventHandler<
    HTMLTextAreaElement | HTMLInputElement
  > = (event) => {
    const { name, value } = event.target;
    setConfig({
      ...config,
      [name]: {
        label: initialState[name as keyof typeof initialState].label,
        value,
      },
    });
  };

  const saveWConfig = () => {
    console.log('[SENDING WIFI CONFIGURATION]');
    window.electron.ipcRenderer.sendToServer(
      JSON.stringify({
        type: 'WIFI_CONFIG',
        data: config,
      })
    );
  };

  console.log('[WIFI CONFIG]', config);
  return { handleWIFI, config, saveWConfig };
};

export type SensorProps = ReturnType<typeof useSensors>;
