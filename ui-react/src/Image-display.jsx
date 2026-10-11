export default function ImageDisplay({
  filteredImage,
  loading,
  handleImageSave,
  handleImageUpload,
  handleResetFilteredImage,
  setDimensions,
  error,
  loadingMessage,
}) {
  return (
    <div className="image-display-container">
      <h2>Apply C++ Image Filters</h2>
      <div className="image-display">
        <div className="img-frame">
          {error && <h3 style={{ color: "red" }}>{error}</h3>}
          {!filteredImage && <h3>Please Load An Image</h3>}
          {loading === true ? <h3>Processing...</h3> : <h3 style={{ color: "green" }}>{loadingMessage}</h3>}
          <div className="image-container" style={{ border: filteredImage ? "none" : "1px dashed black" }}>
            {filteredImage && (
              <img
                src={filteredImage}
                alt=""
                onLoad={e => setDimensions({ width: e.currentTarget.naturalWidth, height: e.currentTarget.naturalHeight })}
              />
            )}
          </div>
        </div>
      </div>
      <div className="options">
        <button onClick={() => handleImageUpload()} disabled={loading}>
          Load Image
        </button>
        <button onClick={() => handleImageSave()} disabled={loading}>
          Save Image
        </button>
        <button onClick={() => handleResetFilteredImage()} disabled={loading}>
          Reset Filters
        </button>
      </div>
    </div>
  );
}
