import { useState } from "react";
export default function MergeMenu({ handelApplyFilter, changeMenu, loading, setLoading, dimensions }) {
  const [mergeImage, setMergeImage] = useState("");
  const [mergeDimensions, setMergeDimensions] = useState({ width: dimensions.width, height: dimensions.height });
  const [isMergeReady, setIsMergeReady] = useState(true);

  async function loadMergeImage() {
    try {
      setLoading(true);
      const filePath = await window.openImagePickerDialog();

      if (filePath === "CANCELLED") {
        return;
      }

      const base64Data = await window.loadImageWithStbAndSave(filePath);

      if (base64Data === "ERROR") {
        return;
      }

      setMergeImage(`data:image/png;base64,${base64Data}`);
    } catch (error) {
      console.error(error);
    } finally {
      setLoading(false);
    }
  }

  return (
    <div className="filter-menu">
      {isMergeReady ? (
        <>
          <p style={{ color: "rgb(193, 153, 160)", marginBlock: "10px", textAlign: "center", paddingInline: "10px" }}>
            Image To Be Merged With Loaded Image
          </p>
          {mergeImage && (
            <>
              <p style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>Image Width: {mergeDimensions.width}</p>
              <p style={{ color: "rgb(193, 153, 160)", marginBlock: "10px" }}>Image Height: {mergeDimensions.height}</p>
            </>
          )}
          <div className="merge-img" style={{ border: mergeImage ? "none" : "1px dashed black" }}>
            {mergeImage && (
              <img
                src={mergeImage}
                alt=""
                onLoad={e => {
                  setMergeDimensions({ width: e.currentTarget.naturalWidth, height: e.currentTarget.naturalHeight });
                }}
              />
            )}
          </div>
          <button onClick={() => loadMergeImage()} disabled={loading}>
            Choose Image
          </button>
          <button
            onClick={() => {
              if (dimensions.width !== mergeDimensions.width || dimensions.height !== mergeDimensions.height) setIsMergeReady(false);
              else handelApplyFilter("Merge", 0);
            }}
            disabled={loading}>
            Merge
          </button>
          <button onClick={() => changeMenu("Filters")} disabled={loading}>
            Back
          </button>
        </>
      ) : (
        <>
          <p style={{ color: "rgb(193, 153, 160)", marginBlock: "10px", textAlign: "center", paddingInline: "10px" }}>
            The Chosen Image Dimensions Differs From The Loaded Image <br />
            Please Choose An Option
          </p>
          <button onClick={() => handelApplyFilter("Merge", 1)} disabled={loading}>
            Loaded Image Dimensions
            <br />
            {dimensions.width}(w) x {dimensions.height}(h)
          </button>
          <button onClick={() => handelApplyFilter("Merge", 2)} disabled={loading}>
            Chosen Merge Image Dimensions
            <br />
            {mergeDimensions.width}(w) x {mergeDimensions.height}(h)
          </button>
          <button onClick={() => handelApplyFilter("Merge", 3)} disabled={loading}>
            Intersection Of Images Dimensions
            <br />
            {Math.min(mergeDimensions.width, dimensions.width)}(w) x {Math.min(mergeDimensions.height, dimensions.height)}(h)
          </button>
          <button onClick={() => setIsMergeReady(true)} disabled={loading}>
            Back
          </button>
        </>
      )}
    </div>
  );
}
