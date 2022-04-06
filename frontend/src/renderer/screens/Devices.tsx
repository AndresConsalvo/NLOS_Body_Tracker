import useStores from '../hooks/useStores';

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
            <tr key={device.ip}>
              <td>{device.id}</td>
              <td>{device.ip}</td>
              <td>{device.battery}</td>
              <td>{device.body_part}</td>
            </tr>
          ))}
        </table>
      </div>
    </>
  );
};
