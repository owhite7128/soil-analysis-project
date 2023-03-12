import React from 'react'
import './header.css';

function Header(props) {
    return (
        <div className='header'>
            <h1>
                {props.scan_id != null ? 
                props.scan_id:
                "Unselected Scan"}
            </h1>
        </div>
    )
}

export default Header