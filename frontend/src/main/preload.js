const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electron', {
  ipcRenderer: {
    myPing() {
      ipcRenderer.send('ipc-example', 'ping');
    },
    python(args) {
      ipcRenderer.send('ipc-python', args);
    },
    sendToServer(args) {
      // console.log('HERE sendToServer', args);
      ipcRenderer.send('ipc-send-to-server', args);
    },
    on(channel, func) {
      const validChannels = ['ipc-example', 'ipc-python'];
      if (validChannels.includes(channel)) {
        // Deliberately strip event as it includes `sender`
        console.log(channel, func);
        ipcRenderer.on(channel, (event, ...args) => {
          console.log('args', new TextDecoder().decode(args[0]))
          return func(...args)
        });
      }
    },
    once(channel, func) {
      const validChannels = ['ipc-example'];
      if (validChannels.includes(channel)) {
        // Deliberately strip event as it includes `sender`
        ipcRenderer.once(channel, (event, ...args) => func(...args));
      }
    },
  },
});
