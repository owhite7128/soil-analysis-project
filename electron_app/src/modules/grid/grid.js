import React from "react";
import "./grid.css"
import Data from "../data/data";
import { useState, useEffect } from 'react';


const alpha = ['a','b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
'aa', 'ab', 'ac', 'ad', 'ae', 'af', 'ag', 'ah', 'ai', 'aj', 'ak', 'al', 'am', 'an', 'ao', 'ap'];

class Point {
	//int x, y;
	constructor(x,y)
	{
		this.x=x;
		this.y=y;
	}
}

class line {
	//Point p1, p2;
	constructor(p1,p2)
	{
		this.p1=p1;
		this.p2=p2;
	}

};

function onLine(l1, p)
{
	// Check whether p is on the line or not
	if (p.x <= Math.max(l1.p1.x, l1.p2.x)
		&& p.x <= Math.min(l1.p1.x, l1.p2.x)
		&& (p.y <= Math.max(l1.p1.y, l1.p2.y)
			&& p.y <= Math.min(l1.p1.y, l1.p2.y)))
		return true;

	return false;
}

function direction(a, b, c)
{
	let val = (b.y - a.y) * (c.x - b.x)
			- (b.x - a.x) * (c.y - b.y);

	if (val === 0)

		// Colinear
		return 0;

	else if (val < 0)

		// Anti-clockwise direction
		return 2;

	// Clockwise direction
	return 1;
}

function isIntersect(l1, l2)
{
	// Four direction for two lines and points of other line
	let dir1 = direction(l1.p1, l1.p2, l2.p1);
	let dir2 = direction(l1.p1, l1.p2, l2.p2);
	let dir3 = direction(l2.p1, l2.p2, l1.p1);
	let dir4 = direction(l2.p1, l2.p2, l1.p2);

	// When intersecting
	if (dir1 !== dir2 && dir3 !== dir4)
		return true;

	// When p2 of line2 are on the line1
	if (dir1 === 0 && onLine(l1, l2.p1))
		return true;

	// When p1 of line2 are on the line1
	if (dir2 === 0 && onLine(l1, l2.p2))
		return true;

	// When p2 of line1 are on the line2
	if (dir3 === 0 && onLine(l2, l1.p1))
		return true;

	// When p1 of line1 are on the line2
	if (dir4 === 0 && onLine(l2, l1.p2))
		return true;

	return false;
}

function checkInside(poly, n, p)
{

	// When polygon has less than 3 edge, it is not polygon
	if (n < 3)
		return false;

	// Create a point at infinity, y is same as point p
	let tmp=new Point(9999, p.y);
	let exline = new line( p, tmp );
	let count = 0;
	let i = 0;
	do {

		// Forming a line from two consecutive points of
		// poly
		let side = new line( poly[i], poly[(i + 1) % n] );
		if (isIntersect(side, exline)) {

			// If side is intersects exline
			if (direction(side.p1, p, side.p2) === 0)
                {
                    console.log("testing_on_line");
				    return onLine(side, p);
                }
			count++;
		}
		i = (i + 1) % n;
	} while (i !== 0);

	// When count is odd
	return count & 1;
}


function doesIntersect (x1, y1, quad)
{
    let quad_pts = [ new Point(quad[0][0], quad[0][1]), new Point(quad[1][0], quad[1][1]), new Point(quad[2][0], quad[2][1]),  new Point(quad[3][0], quad[3][1])];
    let pt = new Point(x1, y1)
    return checkInside (quad_pts, 4, pt);
}


function GenerateGrid (props) {
    const [isLoading, setLoading] = useState(1);
    const [item, setItem] = useState ();
    useEffect(()=> {
        if (localStorage.getItem("scan_id") !== "none") {
            let db;
            let item = [];
            let request = window.indexedDB.open("TestDB", 2);
            request.onsuccess = (event) => {
            db = event.target.result;
            const scanStore = db.transaction("scans", "readwrite").objectStore("scans");
            if (props.scan_id) {
                item = scanStore.get(props.scan_id);
            item.onsuccess = (event) => {
                setItem (item.result);
                setLoading(0);
            }}
        }
        }
    }, [props]);
    if (isLoading) {
        return <h2>LOADING</h2>
    }else if (localStorage.getItem("scan_id") === "none"){
        return <h2>No Scan Selected</h2>

    } else {
        let grid = [];
        let template = ``;
        let columns = '';
        let rows = '';
        let imgsrc = '/scan_images/' + item.img_name;

        let min_x = 0;
        let max_x = 0;
        let max_y = 0;
        let min_y = 0;
        for (let i=0; i<item.bndry.length; i++)
        {
            if (item.bndry[i][0] > max_x) {
                max_x = item.bndry[i][0];
            }
            if (item.bndry[i][0] < min_x) {
                min_x = item.bndry[i][0];
            }
            if (item.bndry[i][1] > max_y) {
                max_y = item.bndry[i][1];
            }
            if (item.bndry[i][1] < min_y) {
                min_y = item.bndry[i][1];
            }
        }

        let id = 0;
        for (let i=max_y; i > min_y; i--) {
            let row_temp = `"`;
            for (let j=min_x; j < max_x; j++) {
                let isIn = 0;
                let y_coord = i - 0.5;
                let x_coord = j + 0.5;
                isIn = doesIntersect(x_coord, y_coord, item.bndry);
                if (isIn === 1 || isIn === false) {
                    let gridProps = {
                        key: id,
                        grid_area:alpha[id],
                        w: props.width / (max_x - min_x) - 4,  
                        h: props.height / (max_y - min_y) - 4,  
                        data:item.data[id]
                    };
                    grid.push(gridProps);
                    row_temp += ` ${alpha[id]} `;
                    id++;
                } else {
                    row_temp += " . ";
                }
                
            }
            row_temp += `" `;
            template += `${row_temp}`;
        }
        for (let i=0;i<(max_x - min_x);i++) {
            columns += (((props.width / (max_x - min_x))) + "px ");
        }
        for (let i=0;i<(max_y - min_y);i++) {
            rows += (((props.height / (max_y - min_y))) + "px ");
        }

        return (
            <div className="grid_full" style={{backgroundImage:`url(${imgsrc})`,width:props.width, height:props.height,gridTemplateColumns:columns, gridTemplateRows:rows, gridTemplateAreas:template}}>
                {mapGrid(grid)}
            </div>
        )
    }
}

function mapGrid(grid) {
    return grid.map(newGridItem);
}

function newGridItem (gridProps) {
    return <GridItem key={gridProps.key} grid_area={gridProps.grid_area} width={gridProps.w} height={gridProps.h}>{gridProps.data}</GridItem>
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
        <div b={over.toString()} className="grid_item" style={{gridArea:props.grid_area.toString() , width: props.width, height:props.height}} onMouseOver={()=>{mOver()}} onMouseLeave={()=>{mOut()}}>
            {over ? <Data key={1}  id={1} w={props.width} h={props.height} >
                <h4>Data</h4>
                <p>PH: {props.children}</p>
            </Data> : <></>    
        }
        </div>
    )
}

export default GenerateGrid