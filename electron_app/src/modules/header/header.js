import React from 'react'
import './header.css';
import { useState, useEffect } from 'react';


function Header() {
    const [scan_id, set_scan_id] = useState(null);
    useEffect(() => {
        function set() {  
            set_scan_id(localStorage.getItem("scan_id"));  
        }
        window.addEventListener ('storage', set);
        return () => {
            window.removeEventListener('storage', set);
        }
    }, []);

    return (
        <div className='header'>
            <h1>
                {scan_id != null ? 
                scan_id:
                "Unselected Scan"}
            </h1>
        </div>
    )
}

export default Header