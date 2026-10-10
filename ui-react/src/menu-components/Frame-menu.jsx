export default function FrameMenu({ handelApplyFilter, changeMenu, loading }) {
  return (
    <div className="filter-menu">
      <button onClick={() => handelApplyFilter("Frame")} disabled={loading}>
        Normal
      </button>
      <button onClick={() => handelApplyFilter("Fancy")} disabled={loading}>
        Fancy
      </button>
      <button onClick={() => changeMenu("Filters")} disabled={loading}>
        Back
      </button>
    </div>
  );
}
