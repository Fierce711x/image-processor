export default function FlipMenu({ handelApplyFilter, changeMenu, loading }) {
  return (
    <div className="filter-menu">
      <button onClick={() => handelApplyFilter("Flip-Horizontally")} disabled={loading}>
        Flip Horizontally
      </button>
      <button onClick={() => handelApplyFilter("Flip-Vertically")} disabled={loading}>
        Flip Vertically
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
