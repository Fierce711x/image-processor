import { useState } from "react";
export default function CropMenu({ handelApplyFilter, changeMenu, loading }) {
  const [xStart, setXStart] = useState(0);
  const [yStart, setYStart] = useState(0);
  const [xRad, setXRad] = useState(500);
  const [yRad, setYRad] = useState(500);
  return (
    <div className="filter-menu">
      <label htmlFor="x-start" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        start(x): {xStart}
      </label>
      <input type="number" id="x-start" min="0" value={xStart} onChange={e => setXStart(e.target.value)}></input>
      <label htmlFor="y-start" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        start(y): {yStart}
      </label>
      <input type="number" id="y-start" min="0" value={yStart} onChange={e => setYStart(e.target.value)}></input>
      <label htmlFor="x-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        width(x): {xRad}
      </label>
      <input type="number" id="x-radius" min="100" value={xRad} onChange={e => setXRad(e.target.value)}></input>
      <label htmlFor="y-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        height(y): {yRad}
      </label>
      <input type="number" id="y-radius" min="100" value={yRad} onChange={e => setYRad(e.target.value)}></input>
      <button onClick={() => handelApplyFilter("Crop", xStart, yStart, xRad, yRad)} disabled={loading}>
        Crop
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
