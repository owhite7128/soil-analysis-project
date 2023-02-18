import React from 'react'
import './data.css';

function Data(props) {
  let x_off = props.w - 17.5;
  let y_off = props.h + 17.5;

  return (
    <div className='data' id={props.id} style={{top: y_off, left: x_off}}>
        {props.children}
    </div>
  )
}
export default Data