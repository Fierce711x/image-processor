export default function RotateMenu({ handelApplyFilter, changeMenu, loading }) {
  return (
    <div className="filter-menu">
      <button onClick={() => handelApplyFilter("Rotate", 90)} disabled={loading}>
        90 Degrees
      </button>
      <button onClick={() => handelApplyFilter("Rotate", 180)} disabled={loading}>
        180 Degrees
      </button>
      <button onClick={() => handelApplyFilter("Rotate", 270)} disabled={loading}>
        270 Degrees
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
