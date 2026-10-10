import { useState } from "react";
export default function CropMenu({ handelApplyFilter, changeMenu, loading, dimensions }) {
  const [xStart, setXStart] = useState(0);
  const [yStart, setYStart] = useState(0);
  const [xRad, setXRad] = useState(500);
  const [yRad, setYRad] = useState(500);
  const imageWidth = dimensions.width;
  const imageHeight = dimensions.height;
  return (
    <div className="filter-menu">
      <p style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>Image Width: {imageWidth}</p>
      <p style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>Image Height: {imageHeight}</p>
      <label htmlFor="x-start" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        start(x): {xStart == "" ? 0 : xStart}
      </label>
      <input
        type="number"
        id="x-start"
        min="0"
        max={imageWidth}
        value={xStart}
        onChange={e => {
          if (e.target.value === "") setXStart("");
          else {
            let value = Number(e.target.value);
            const max = e.target.max;
            const min = e.target.min;

            if (value > max) {
              value = max;
            } else if (value < min) {
              value = min;
            }
            setXStart(value);
            if (imageWidth - value < xRad) {
              setXRad(imageWidth - value);
            }
          }
        }}></input>
      <label htmlFor="y-start" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        start(y): {yStart == "" ? 0 : yStart}
      </label>
      <input
        type="number"
        id="y-start"
        min="0"
        max={imageHeight}
        value={yStart}
        onChange={e => {
          if (e.target.value === "") setYStart("");
          else {
            let value = Number(e.target.value);
            const max = e.target.max;
            const min = e.target.min;

            if (value > max) {
              value = max;
            } else if (value < min) {
              value = min;
            }
            setYStart(value);
            if (imageHeight - value < yRad) {
              setYRad(imageHeight - value);
            }
          }
        }}></input>
      <label htmlFor="x-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        width(x): {xRad == "" ? 0 : xRad} (max: {imageWidth - xStart})
      </label>
      <input
        type="number"
        id="x-radius"
        min="0"
        max={imageWidth}
        value={xRad}
        onChange={e => {
          if (e.target.value === "") setXRad("");
          else {
            let value = Number(e.target.value);
            const max = e.target.max;
            const min = e.target.min;

            if (value > max) {
              value = max;
            } else if (value < min) {
              value = min;
            }
            setXRad(value);
            if (imageWidth - value < xStart) {
              setXStart(imageWidth - value);
            }
          }
        }}></input>
      <label htmlFor="y-radius" style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>
        height(y): {yRad == "" ? 0 : yRad} (max: {imageHeight - yStart})
      </label>
      <input
        type="number"
        id="y-radius"
        min="0"
        max={imageHeight}
        value={yRad}
        onChange={e => {
          if (e.target.value === "") setYRad("");
          else {
            let value = Number(e.target.value);
            const max = e.target.max;
            const min = e.target.min;

            if (value > max) {
              value = max;
            } else if (value < min) {
              value = min;
            }
            setYRad(value);
            if (imageHeight - value < yStart) {
              setYStart(imageHeight - value);
            }
          }
        }}></input>
      <button
        onClick={() =>
          handelApplyFilter("Crop", xStart == "" ? 0 : xStart, yStart == "" ? 0 : yStart, xRad == "" ? 0 : xRad, yRad == "" ? 0 : yRad)
        }
        disabled={loading}>
        Crop
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
