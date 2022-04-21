import { createContext, ReactNode } from 'react';
import { useMeasurements } from './useMeasurements';
import { SensorProps, useSensors } from './useSensors';
import { useWConfig } from './useWConfig';

interface Props {
  children: ReactNode;
}

type AppContextType = {
  sensors: SensorProps;
  measurementStore: ReturnType<typeof useMeasurements>;
  wConfigStore: ReturnType<typeof useWConfig>;
};

export const AppContext = createContext<AppContextType>({} as AppContextType);

export const AppProvider = ({ children }: Props) => {
  const sensors = useSensors();
  const measurementStore = useMeasurements();
  const wConfigStore = useWConfig();
  return (
    <AppContext.Provider
      value={{
        sensors,
        measurementStore,
        wConfigStore,
      }}
    >
      {children}
    </AppContext.Provider>
  );
};
