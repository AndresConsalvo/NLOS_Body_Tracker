import { Button, CircularProgress, TextField } from '@mui/material';
import React, { useEffect, useRef, useState } from 'react';
import { useMeasurements } from 'renderer/hooks/useMeasurements';
import useStores from '../hooks/useStores';

export default () => {
  const { sensors, measurementStore } = useStores();
  const { devices, changeRole } = sensors;
  const { handleText, measurements, saveBodyMeasurements } = measurementStore;

  const [calibrating, setCalibrating] = useState(false);
  const intervalRef = useRef<NodeJS.Timeout | null>(null);
  const [calibrated, setCalibrated] = useState(false);
  useEffect(() => {
    return () => {
      if (intervalRef.current) clearTimeout(intervalRef.current);
    };
  }, []);

  const handleCalibration = () => {
    setCalibrating(true);
    setCalibrated(false);
    intervalRef.current = setTimeout(() => {
      window.electron.ipcRenderer.sendToServer(
        JSON.stringify({
          type: 'CALIBRATE',
          data: 'calibrate',
        })
      );

      setCalibrating(false);
      setCalibrated(true);
    }, 8000);
  };

  const handleChange: React.ChangeEventHandler<HTMLSelectElement> = (e) => {
    changeRole(devices[e.target.name], e.target.value);
  };

  return (
    <div className="main">
      <div className="current">
        {/* <button onClick={handleClick}>ADD new role</button> */}
        <h1>Device Settings</h1>
      </div>
      <table>
        <tr>
          <th>ID</th>
          <th>Connect</th>
          <th>Role</th>
        </tr>
        {Object.values(devices).map((device) => (
          <tr>
            <td>{device.id}</td>
            <td>
              <label className="switch">
                <input type="checkbox" />
                <span className="slider"></span>
              </label>
            </td>
            <td>
              <select onChange={handleChange} name={device.id} id="Roles">
                <option value="blank">Choose a position:</option>
                <option value="Right Ankle">Right Ankle</option>
                <option value="Left Ankle">Left Ankle</option>
                <option value="Right Knee">Right Knee</option>
                <option value="Left Knee">Left Knee</option>
                <option value="Chest">Chest</option>
                <option value="Back">Back</option>
              </select>
            </td>
          </tr>
        ))}
      </table>
      <h3>Set Body Measurements: (centimeters)</h3>
      <div style={{ display: 'flex', flexDirection: 'row' }}>
        <div style={{ display: 'flex', flexDirection: 'column' }}>
          {Object.entries(measurements).map((entry) => {
            const [key, input] = entry;
            return (
              <TextField
                id="filled-basic"
                label={input.label}
                variant="filled"
                value={input.value}
                key={key}
                name={key}
                onChange={handleText}
                type="number"
                style={{
                  marginBottom: '10px',
                  width: '200px',
                  display: 'inline',
                }}
              />
            );
          })}
        </div>
        <div style={{ marginLeft: '30px' }}>
          <Button
            variant="contained"
            color="success"
            onClick={saveBodyMeasurements}
          >
            Save Measurements
          </Button>
        </div>
      </div>
      <h3>Calibrate Trackers:</h3>

      {calibrating ? (
        <>
          <CircularProgress style={{ marginLeft: '56px' }} />
          <p>Stand still in a "T" pose.</p>
          <img
            src="https://img.icons8.com/ios/452/t-pose.png"
            height={'200px'}
          />
        </>
      ) : (
        <Button variant="contained" onClick={handleCalibration}>
          Calibrate
        </Button>
      )}
      {calibrated && <p>Finished calibration</p>}
    </div>
  );
};
