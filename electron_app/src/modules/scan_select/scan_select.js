import React from 'react';
import './scan_select.css';
import { useState, useEffect } from "react";


function selectedOption (props, val) {
    props.parentCallback(val);
    localStorage.setItem("scan_id", val);
    if (val !== "none") {
        let request = window.indexedDB.open("TestDB", 2);
        request.onsuccess = (event) => {
            const db = event.target.result;

            const scanStore = db.transaction("scans", "readwrite").objectStore("scans");
            const item = scanStore.get(val);
            item.onsuccess = (event) => {
                console.log (`Item is: ${item.result.date}`);
                console.log (item.result);
            }
        }
    }
}

function returnOption (val) {
    return <option>{val.date}</option>
}

function returnOptions (vals) {
    return vals.map(returnOption);
}

function ScanSelect(props) {
    
    const [isLoading, setLoading] = useState(1);
    const [itemState, setItems] = useState();
    useEffect (()=> {
        let db;
        let items = [];
        let request = window.indexedDB.open("TestDB", 2);
        request.onsuccess = (event) => {
        db = event.target.result;
        const scanStore = db.transaction("scans", "readwrite").objectStore("scans");
        items = scanStore.getAll();
        items.onsuccess = (event) => {
            setLoading(0);
            setItems(items.result);
        }
        props.parentCallback("-1");
    }
    }, [props]);
    
    if (!isLoading) {
        return (
            <div className='scan_select'>
                <label htmlFor="scan_select">Select What Scan: </label>
                <select onChange={(event) => {selectedOption(props, event.target.value)}} name="Select What Scan" id="scan_select">
                    <option >none</option>
                    {   
                        returnOptions(itemState)
                    }
                </select>
            </div>
        )
    } else {
        return (
            <div className='scan_select'>
                <label htmlFor="scan_select">Select What Scan: </label>
                <select onChange={(event) => {selectedOption(props, event.target.value)}} name="Select What Scan" id="scan_select">
                    <option>none</option>
                    <option>Loading Scans</option>
                </select>
            </div>
        )
    }
}

export default ScanSelect