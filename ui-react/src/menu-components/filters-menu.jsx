export default function FiltersMenu({ handelApplyFilter, changeMenu, loading }) {
  return (
    <div className="filter-menu">
      <button onClick={() => handelApplyFilter("Invert")} disabled={loading}>
        Invert Colors
      </button>
      <button onClick={() => changeMenu("Flip")}>Flip</button>
      <button onClick={() => handelApplyFilter("Grayscale")} disabled={loading}>
        Grayscale
      </button>
      <button onClick={() => handelApplyFilter("Black-&-White")} disabled={loading}>
        Black & White
      </button>
      <button onClick={() => handelApplyFilter("Purple-Tint")} disabled={loading}>
        Purple Tint
      </button>
      <button onClick={() => handelApplyFilter("Infrared")} disabled={loading}>
        Infrared
      </button>
      <button onClick={() => handelApplyFilter("Old-Television")} disabled={loading}>
        Old Television
      </button>
      <button onClick={() => changeMenu("Lighten")} disabled={loading}>
        Lighten Brightness
      </button>
      <button onClick={() => changeMenu("Darken")} disabled={loading}>
        Darken Brightness
      </button>
      <button onClick={() => changeMenu("Rotate")} disabled={loading}>
        Rotate Image
      </button>
      <button onClick={() => handelApplyFilter("Sunny")} disabled={loading}>
        Sunny
      </button>
      <button onClick={() => handelApplyFilter("Merge-2-Images")} disabled={loading}>
        Merge 2 Images
      </button>
      <button onClick={() => changeMenu("Resize")} disabled={loading}>
        Resize
      </button>
      <button onClick={() => changeMenu("Blur")} disabled={loading}>
        Blur
      </button>
      <button onClick={() => handelApplyFilter("Edge-Detection")} disabled={loading}>
        Edge Detection
      </button>
      <button onClick={() => changeMenu("Crop")} disabled={loading}>
        Crop Image
      </button>
    </div>
  );
}
