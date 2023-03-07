import React, { useEffect } from 'react';
import './scan_select.css';

const setLocal = () => {
    let val = document.getElementById("scan_select").value;
    localStorage.setItem("scan_id", val);
    return 0;
}


function ScanSelect() {
  return (
    <div className='scan_select'>
        <label htmlFor="scan_select">Select What Scan: </label>
        <select onChange={() => {setLocal()}} name="Select What Scan" id="scan_select">
            <option >none</option>
            <option >2_12_23_a</option>
            <option >2_12_23_b</option>
            <option >2_12_23_c</option>
        </select>
    </div>
  )
}

export default ScanSelect