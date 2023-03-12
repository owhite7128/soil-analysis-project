import React from 'react'
import './data.css';

function Data(props) {
  //let x_off = props.w - (3 * props.w / 4);
  //let y_off = props.h - (3 * props.h / 4);

  return (
    <div className='data' id={props.id} style={{/*top: y_off, left: x_off*/}}>
        {props.children}
    </div>
  )
}
export default Data