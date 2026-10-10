import { useState } from "react";
export default function BlurMenu({ handelApplyFilter, changeMenu, loading }) {
  const [xRad, setXRad] = useState(3);
  const [yRad, setYRad] = useState(3);
  return (
    <div className="filter-menu">
      <label htmlFor="x-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        x-radius: {xRad == "" ? "0" : xRad} (max: 20)
      </label>
      <input
        type="number"
        id="x-radius"
        min="0"
        max="20"
        value={xRad}
        onChange={e => {
          if (e.target.value === "") setXRad("");
          else {
            const value = Number(e.target.value);
            const max = e.target.max;
            const min = e.target.min;
            if (value > max) {
              setXRad(max);
            } else if (value < min) {
              setXRad(min);
            } else {
              setXRad(value);
            }
          }
        }}></input>
      <label htmlFor="y-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        y-radius: {yRad == "" ? "0" : yRad} (max: 20)
      </label>
      <input
        type="number"
        id="y-radius"
        min="0"
        max="20"
        value={yRad}
        onChange={e => {
          if (e.target.value === "") setYRad("");
          else {
            const value = Number(e.target.value);
            const max = e.target.max;
            const min = e.target.min;
            if (value > max) {
              setYRad(max);
            } else if (value < min) {
              setYRad(min);
            } else {
              setYRad(value);
            }
          }
        }}></input>
      <button onClick={() => handelApplyFilter("Blur", xRad, yRad)} disabled={loading}>
        Blur
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
