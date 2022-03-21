import { useSensors } from '../hooks/useSensors';

interface Props extends ReturnType<typeof useSensors> {}
export default (props: Props) => {
  const { devices, changeRole } = props;
  const handleClick = () => {
    try {
      changeRole(Object.values(devices)[0], 'New role');
    } catch (error) {
      console.error(error);
      console.log('Tried to add a role to non existing tracker');
    }
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
              <select name="Roles" id="Roles">
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
    </div>
  );
};
