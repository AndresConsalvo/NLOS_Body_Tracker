import { useSensors } from '../hooks/useSensors';

interface Props extends ReturnType<typeof useSensors> {}
export default (props: Props) => {
  const { devices } = props;
  return (
    <>
      <div className="main">
        <div className="current">
          <h1>Data</h1>
        </div>

        <table>
          <tr>
            <th>Tracker</th>
            <th>Px</th>
            <th>Py</th>
            <th>Pz</th>
            <th>Gx</th>
            <th>Gy</th>
            <th>Gz</th>
          </tr>
          {Object.values(devices).map((device) => (
            <tr key={device.ip}>
              <th>{device.id}</th>
              {[...device.accel, ...device.gyro].map((position) => (
                <th key={position}>{position.toFixed(2)}</th>
              ))}
            </tr>
          ))}
        </table>
      </div>
    </>
  );
};
