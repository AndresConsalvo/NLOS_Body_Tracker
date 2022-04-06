import { createContext, ReactNode } from 'react';
import { useMeasurements } from './useMeasurements';
import { SensorProps, useSensors } from './useSensors';

interface Props {
  children: ReactNode;
}

type AppContextType = {
  sensors: SensorProps;
  measurementStore: ReturnType<typeof useMeasurements>;
};

export const AppContext = createContext<AppContextType>({} as AppContextType);

export const AppProvider = ({ children }: Props) => {
  const sensors = useSensors();
  const measurementStore = useMeasurements();
  return (
    <AppContext.Provider
      value={{
        sensors,
        measurementStore,
      }}
    >
      {children}
    </AppContext.Provider>
  );
};
