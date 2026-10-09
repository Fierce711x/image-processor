import { useState } from "react";
export default function BlurMenu({ handelApplyFilter, changeMenu, loading }) {
  const [xRad, setXRad] = useState(3);
  const [yRad, setYRad] = useState(3);
  return (
    <div className="filter-menu">
      <label htmlFor="x-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        x-radius: {xRad}
      </label>
      <input type="number" id="x-radius" min="0" value={xRad} onChange={e => setXRad(e.target.value)}></input>
      <label htmlFor="y-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        y-radius: {yRad}
      </label>
      <input type="number" id="y-radius" min="0" value={yRad} onChange={e => setYRad(e.target.value)}></input>
      <button onClick={() => handelApplyFilter("Blur", xRad, yRad)} disabled={loading}>
        Blur
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
