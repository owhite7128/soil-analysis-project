import './App.css';
import Map from './modules/map/map';
import ScanSelect from './modules/scan_select/scan_select';
import Header from './modules/header/header';
import { useState } from 'react';
import scans from './scans.json';


function syncDB (scans) {
  const request = window.indexedDB.open("TestDB", 2);
  request.onerror = (event) => {
    console.error("Why didn't you allow my web app to use IndexedDB?!");
  };
  request.onsuccess = (event) => {
    const db = event.target.result;
    const scanObjectStore = db
      .transaction(["scans"], "readwrite")
      .objectStore("scans");
    scans.scans.forEach((scan) => {
      let storeReq = scanObjectStore.put(scan);
      storeReq.onerror = (event) => {
        console.log(event);
      }
      storeReq.onsuccess = (event) => {
        console.log ("success");
      }
    });
  }
  return 0;
} 

function App() {
  const [scan_id, set_scan_id] = useState(null);
  const [isSync, setSync] = useState(0);
  let db;
  const request = window.indexedDB.open("TestDB", 2);
  request.onerror = (event) => {
    console.error("Why didn't you allow my web app to use IndexedDB?!");
  };
  request.onupgradeneeded = (event) => {
    db = event.target.result;

    const scanStore = db.createObjectStore ("scans", {keyPath: "date"});

    scanStore.createIndex ("img_name", "img_name", {unique: true});
    scanStore.createIndex ("bndry", "bndry", {unique: false});
    scanStore.createIndex ("data", "data", {unique: false});
  };
  
  return (
    <div className="App">
      <button id="sync_btn" onClick={(e)=>{setSync(1); syncDB (scans)}}>SYNC</button>
      <Header scan_id={scan_id}></Header>
      <div className="app-main">
        <ScanSelect sync={isSync} parentCallback = {(data)=>{
          if (data === "-1") {
            setSync(0)
          }else {
            set_scan_id(data)}
          }}></ScanSelect>
        <Map scan_id={scan_id}></Map>
      </div>
    </div>
  );
}

export default App;
