import { useState } from "react";
export default function ResizeMenu({ handelApplyFilter, changeMenu, loading }) {
  const [xRad, setXRad] = useState(500);
  const [yRad, setYRad] = useState(500);
  return (
    <div className="filter-menu">
      <label htmlFor="x-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        width(x): {xRad}
      </label>
      <input type="number" id="x-radius" min="0" value={xRad} onChange={e => setXRad(e.target.value)}></input>
      <label htmlFor="y-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        height(y): {yRad}
      </label>
      <input type="number" id="y-radius" min="0" value={yRad} onChange={e => setYRad(e.target.value)}></input>
      <button onClick={() => handelApplyFilter("Resize", xRad, yRad)} disabled={loading}>
        Resize
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
