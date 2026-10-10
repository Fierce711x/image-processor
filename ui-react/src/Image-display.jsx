export default function ImageDisplay({
  filteredImage,
  loading,
  handleImageSave,
  handleImageUpload,
  handleResetFilteredImage,
  setDimensions,
}) {
  return (
    <div className="image-display-container">
      <h2>Apply C++ Image Filters</h2>
      <div className="image-display">
        <div className="img-frame">
          {!filteredImage && <h3>Please Load An Image</h3>}
          {loading && <h3>Processing...</h3>}
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
