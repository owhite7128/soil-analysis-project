const electron = require("electron");
const { ipcMain } = require("electron")
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;
const path = require("path");
const isDev = require("electron-is-dev");


let mainWindow;
app.commandLine.appendSwitch("enable-experimental-web-platform-features", true);
app.commandLine.appendSwitch("enable-web-bluetooth", true);
if (process.platform === "linux"){
  app.commandLine.appendSwitch("enable-experimental-web-platform-features", true);
} else {
  app.commandLine.appendSwitch("enable-web-bluetooth", true);
}

function createWindow() {
mainWindow = new BrowserWindow({ width: 900, height: 680, webPreferences: {
    preload: path.join(__dirname, 'preload.js'), nodeIntegration: true}});
    mainWindow.webContents.on(
      "select-bluetooth-device",
      (event, deviceList, callback) => 
      {
        event.preventDefault(); // do not choose the first one
        if (deviceList && deviceList.length > 0) {
          console.log("ID: "+  deviceList[0].deviceId + " | Name: " + deviceList[0].deviceName)
          callback(deviceList[0].deviceId)
        } 
      })
    // Listen for a message from the renderer to get the response for the Bluetooth pairing.
  ipcMain.on('bluetooth-pairing-response', (event, response) => {
    bluetoothPinCallback(response)
  })

  mainWindow.webContents.session.setBluetoothPairingHandler((details, callback) => {
    bluetoothPinCallback = callback
    // Send a message to the renderer to prompt the user to confirm the pairing.
    mainWindow.webContents.send('bluetooth-pairing-request', details)
  })  
mainWindow.loadURL(
isDev
? "http://localhost:3000"
: `file://${path.join(__dirname, "../build/index.html")}`
);
mainWindow.on("closed", () => (mainWindow = null));
}

app.on("ready", createWindow);
app.on("window-all-closed", () => {
if (process.platform !== "darwin") {
app.quit();
}
});
app.on("activate", () => {
if (mainWindow === null) {
createWindow();
}
});
