import { useContext } from 'react';
import { AppContext } from './useAppContext';

export default () => {
  const ctx = useContext(AppContext);
  return ctx;
};
