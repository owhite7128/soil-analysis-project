import React from "react";
import "./grid.css"
import Data from "../data/data";
import { useState } from 'react';

function GenerateGrid (props) {
    let grid = [];
    let columns = '';
    let rows = '';
    for (let i=0;i<props.n; i++) {
        for (let j=0; j<props.n; j++){
            let gridProps = {
                i:i,
                j:j,
                w:props.width / props.n,
                h:props.height / props.n,
                data:j + " " + i
            };
            grid.push(gridProps);
        }
    }
    for (let i=0;i<props.n;i++) {
        columns += (((props.width / props.n)) + "px ");
    }
    for (let i=0;i<props.n;i++) {
        rows += (((props.height / props.n)) + "px ");
    }

    return (
        <div className="grid_full" style={{gap:2,gridTemplateColumns:columns, gridTemplateRows:rows}}>
            {mapGrid(grid)}
        </div>
    )
}

function mapGrid(grid) {
    return grid.map(newGridItem);
}

function newGridItem (gridProps) {
    return <GridItem a={gridProps.j} b={gridProps.i} width={gridProps.w} height={gridProps.h}>{gridProps.data}</GridItem>
}

function GridItem (props) {
    const [over, setOver] = useState(false);
    const mOver = () => {
        setOver(true);
    }
    const mOut = () => {
        setOver(false);
    }

    return (
        <div b={over.toString()} className="grid_item" style={{width:props.width, height:props.height}} onMouseOver={()=>{mOver()}} onMouseLeave={()=>{mOut()}}>
            {over ? <Data key={1} x={props.a} y={props.b} id={1} w={props.width} h={props.height}>
                <h4>Data</h4>
                <p>PH: </p>
                <p>Hydration: </p>
                <p>Nitrogen Level: </p>
            </Data> : <></>    
        }
        </div>
    )
}

export default GenerateGrid