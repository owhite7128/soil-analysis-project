// First determine if using MySQL or Postgres or SQLite
// For Now Build N x N Grid
import React from 'react'
import GenerateGrid from '../grid/grid';
import './map.css'

function Map() {
    const w = 300;
    const h = 300;
    const n = 8;
    return (
        <div className='map'>
            <GenerateGrid n={n} width={w} height={h} className="map_grid"></GenerateGrid>
        </div>
    )
}

export default Map