import useStores from '../hooks/useStores';

export default () => {
  const { sensors } = useStores();
  const { devices } = sensors;
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
            <tr key={device.id}>
              <th>{device.id}</th>
              {[...device.accel, ...device.gyro].map((position, index) => (
                // eslint-disable-next-line react/no-array-index-key
                <th key={index + device.id}>{position.toFixed(2)}</th>
              ))}
            </tr>
          ))}
        </table>
      </div>
    </>
  );
};
