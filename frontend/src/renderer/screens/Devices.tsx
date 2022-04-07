import useStores from '../hooks/useStores';

const bp = {
  1: 'Chest',
  2: 'Waist',
  3: 'Left Knee',
  4: 'Right Knee',
  5: 'Left Ankle',
  6: 'Right Ankle',
};

export default () => {
  const { sensors } = useStores();
  const { devices } = sensors;

  return (
    <>
      <div className="main">
        <div className="current">
          <h1>Devices</h1>
        </div>

        <table>
          <tr>
            <th>ID</th>
            <th>IP Address</th>
            <th>Battery (%)</th>
            <th>Body Part</th>
          </tr>
          {Object.values(devices).map((device) => (
            <tr key={device.id}>
              <td>{device.id}</td>
              <td>{device.ip}</td>
              <td>{device.battery}</td>
              <td>{bp[device.body_part]}</td>
            </tr>
          ))}
        </table>
      </div>
    </>
  );
};
