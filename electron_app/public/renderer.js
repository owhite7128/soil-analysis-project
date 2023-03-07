async function testIt() {
  let options = {
    //acceptAllDevices: true
    filters: [
      { name: "Echo" },
    ],
    optionalServices: ['battery_service']
  };
  await navigator.bluetooth.requestDevice(options)
  .then ((device) => {
    console.log ("Id: " + device.id + " | Name: " + device.name);
    return device.gatt.connect()
  }).then((server) => {
    return server.getPrimaryService ('battery_service');
  }).then ((service) => {
    console.log (service);
    return service.getCharacteristic ('battery_level');
  }).then((characteristic) => {
    console.log (characteristic);
    return characteristic.readValue();
  }).then((val) => {
    console.log (`Battery percentage is ${val.getUint8(0)}`);
  }).catch((err) => {console.error ("Something went wrong: " + err)});
  
}

document.getElementById('click_btn').addEventListener('click',testIt)

window.electronAPI.bluetoothPairingRequest((event, details) => {
  const response = {}
  
  switch (details.pairingKind) {
    default: {
      console.log ("I have Cancer!!");
      break
    }
    case 'confirm': {
      response.confirmed = confirm(`Do you want to connect to device ${details.deviceId}?`)
      break
    }
    case 'confirmPin': {
      response.confirmed = confirm(`Does the pin ${details.pin} match the pin displayed on device ${details.deviceId}?`)
      break
    }
    case 'providePin': {
      const pin = prompt(`Please provide a pin for ${details.deviceId}.`)
      if (pin) {
        response.pin = pin
        response.confirmed = true
      } else {
        response.confirmed = false
      }
    }
  }

  window.electronAPI.bluetoothPairingResponse(response)
})