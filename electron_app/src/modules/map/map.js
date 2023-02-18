// First determine if using MySQL or Postgres or SQLite
// For Now Build N x N Grid
import React from 'react'
import GenerateGrid from '../grid/grid';
import './map.css'

function Map() {
    const w = 300;
    const h = 300;
    const n = 8;
    let date = "02/12/23_a"
    return (
        <div className='map'>
            <h2>Current Selected Scan: {date}</h2>
            <GenerateGrid n={n} width={w} height={h} className="map_grid"></GenerateGrid>
        </div>
    )
}

export default Map