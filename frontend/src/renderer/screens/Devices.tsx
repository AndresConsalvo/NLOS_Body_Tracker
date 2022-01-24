import { useSensors } from '../hooks/useSensors';

interface Props extends ReturnType<typeof useSensors> {}

export default (props: Props) => {
  const { devices } = props;
  return (
    <>
      <div className="main">
        <div className="current">
          <h1>Devices</h1>
        </div>

        <table>
          <tr>
            <th>MAC Address</th>
            <th>IP Address</th>
            <th>Battery (%)</th>
            <th>Battery (V)</th>
            <th>IMU Accuracy</th>
            <th>Tracker</th>
          </tr>
          {Object.values(devices).map((device) => (
            <tr key={device.ip}>
              <td>{device.MAC_ADRESS}</td>
              <td>{device.ip}</td>
              <td>{device.battery}</td>
              <td>{device.battery_V}</td>
              <td>{device.IMU_accuracy}</td>
              <td>{device.role}</td>
            </tr>
          ))}
        </table>
      </div>
    </>
  );
};
