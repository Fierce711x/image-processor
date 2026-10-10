import { useState } from "react";
export default function SkewMenu({ handelApplyFilter, changeMenu, loading }) {
  const [angle, setAngle] = useState(0);
  return (
    <div className="filter-menu">
      <label htmlFor="deg" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        Adjust degree: {(angle == "" ? "0" : angle) + String.fromCharCode(176)}
      </label>
      <input
        type="number"
        id="deg"
        min="-89"
        max="89"
        value={angle}
        onChange={e => {
          if (e.target.value === "") setAngle("");
          else {
            const value = Number(e.target.value);
            const max = e.target.max;
            const min = e.target.min;
            if (value > max) {
              setAngle(max);
            } else if (value < min) {
              setAngle(min);
            } else {
              setAngle(value);
            }
          }
        }}></input>
      <button onClick={() => handelApplyFilter("Skew", angle == "" ? "0" : angle)} disabled={loading}>
        Skew
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
