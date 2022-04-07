import { useEffect, useState } from 'react';

type ServerEvent<T = unknown> = {
  type: string;
  data: T;
};

const initialState = {
  headToNeck: {
    label: 'Head to Neck',
    value: '',
  },
  neckToChest: {
    label: 'Neck to Chest',
    value: '',
  },
  chestToWaist: {
    label: 'Chest to Waist',
    value: '',
  },
  hipWidth: {
    label: 'Hip Width',
    value: '',
  },
  hipToKnee: {
    label: 'Hip to Knee',
    value: '',
  },
  kneeToFoot: {
    label: 'Knee to Foot',
    value: '',
  },
};

export const useMeasurements = () => {
  const [measurements, setMeasurements] = useState(initialState);

  const handleText: React.ChangeEventHandler<
    HTMLTextAreaElement | HTMLInputElement
  > = (event) => {
    const { name, value } = event.target;
    setMeasurements({
      ...measurements,
      [name]: {
        label: initialState[name as keyof typeof initialState].label,
        value,
      },
    });
  };

  const saveBodyMeasurements = () => {
    console.log('[SENDING MEASUREMENTS TO SERVER]');
    window.electron.ipcRenderer.sendToServer(
      JSON.stringify({
        type: 'BODY_MEASUREMENTS',
        data: measurements,
      })
    );
  };

  console.log('[MEASUREMENT]', measurements);
  return { handleText, measurements, saveBodyMeasurements };
};

// const postMeasurements = (sensor: Sensor, newRole: string) => {
//   window.electron.ipcRenderer.sendToServer(
//     JSON.stringify({
//       type: 'BODY_MEASURES',
//       data: {
//         ip: sensor.ip,
//         body_part: newRole,
//       },
//     })
//   );
// };

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

export type SensorProps = ReturnType<typeof useSensors>;
