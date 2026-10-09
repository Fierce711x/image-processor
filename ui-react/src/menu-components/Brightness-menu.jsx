import { useState } from "react";
export default function BrightnessMenu({ handelApplyFilter, changeMenu, type, loading }) {
  const [level, setLevel] = useState(50);
  return (
    <div className="filter-menu">
      <label htmlFor="range-progress" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        Adjust value: {level}
      </label>
      <input type="range" id="range-progress" min="0" max="100" value={level} onChange={e => setLevel(e.target.value)}></input>
      <button onClick={() => handelApplyFilter(type, (level / 100) * 255)} disabled={loading}>
        {type} Brightness
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
