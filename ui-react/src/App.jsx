import { useState, useRef } from "react";
import luffy from "../../luffy.jpg";
function App() {
  const [loadedImage, setLoadedImage] = useState("");
  const loadedImageStatusRef = useRef();
  const [filteredImage, setFilteredImage] = useState("");
  const filteredImageStatusRef = useRef();
  async function handleImageUpload() {
    setLoadedImage(luffy);
    setFilteredImage(luffy);
    try {
      // loadedImageStatusRef.current.innerText = "Opening file picker...";
      const filePath = await window.openImagePickerDialog();

      if (filePath === "CANCELLED") {
        // loadedImageStatusRef.current.innerText = "Selection cancelled.";
        return;
      }

      // loadedImageStatusRef.current.innerText = `Loading: ${filePath}`;

      const base64Data = await window.loadImageWithStb(filePath);

      if (base64Data === "ERROR") {
        // loadedImageStatusRef.current.innerText = "Error parsing image file with stb_image.";
        return;
      }

      setLoadedImage(`data:image/png;base64,${base64Data}`);
      setFilteredImage(`data:image/png;base64,${base64Data}`);
      // loadedImageStatusRef.current.innerText = "Image successfully loaded!";
      // filteredImageStatusRef.current.innerText = "Image successfully loaded!";
    } catch (error) {
      // loadedImageStatusRef.current.innerText = "An unexpected error occurred.";
      console.error(error);
    }
  }
  async function handleImageSave() {
    try {
      const result = await window.saveActiveImageDialog();
      if (result === "CANCELLED") {
        throw new Error("CANCELLED");
      } else if (result === "WRITE_FAILED") {
        throw new Error("WRITE_FAILED");
      } else if (result === "SUCCESS") {
        console.log("SUCCESS");
      } else {
        throw new Error("unexpected error");
      }
    } catch (error) {
      console.error(error);
    }
  }
  async function handelApplyFilter(filter) {
    try {
      const status = await window.applyFilter(filter);
      if (status !== "SUCCESS") throw new Error("filter not applied");
      const base64String = await window.getImageBase64();
      setFilteredImage(`data:image/png;base64,${base64String}`);
      // filteredImageStatusRef.current.innerText = "Filter Successfully Applied!";
    } catch (error) {
      // filteredImageStatusRef.current.innerText = "An unexpected error occured.";
      console.log(error);
    }
  }

  async function handleResetFilteredImage() {
    try {
      const status = await window.resetFilteredImage();
      if (status !== "SUCCESS") throw new Error("image not reset");
      setFilteredImage(loadedImage);
    } catch (error) {
      console.log(error);
    }
  }
  return (
    <>
      <div className="container">
        <div className="image-display-container">
          <h2>Apply C++ Image Filters</h2>
          <div className="image-display">
            <div className="img-frame">
              <h3 ref={loadedImageStatusRef}>Loaded Image</h3>
              <div className="image-container" style={{ border: loadedImage ? "none" : "1px dashed black" }}>
                {loadedImage && <img src={loadedImage} alt="" />}
              </div>
            </div>
            <div className="img-frame">
              <h3 ref={filteredImageStatusRef}>Filtered Image</h3>
              <div className="image-container" style={{ border: loadedImage ? "none" : "1px dashed black" }}>
                {filteredImage && <img src={filteredImage} alt="" />}
              </div>
            </div>
          </div>
          <div className="options">
            <button onClick={() => handleImageUpload()}>Load Image</button>
            <button onClick={() => handleImageSave()}>Save Image</button>
            <button onClick={() => handleResetFilteredImage()}>Reset Filters</button>
          </div>
        </div>
        <div className="menu">
          <h2>Filters</h2>
          <div className="filter-menu">
            <button onClick={() => handelApplyFilter("Invert")}>Invert Colors</button>
            <button onClick={() => handelApplyFilter("Flip Horizontally")}>Flip Horizontally</button>
            <button onClick={() => handelApplyFilter("Flip Vertically")}>Flip Vertically</button>
            <button onClick={() => handelApplyFilter("Grayscale")}>Grayscale</button>
            <button onClick={() => handelApplyFilter("Black & White")}>Black & White</button>
            <button onClick={() => handelApplyFilter("Purple Tint")}>Purple Tint</button>
            <button onClick={() => handelApplyFilter("Infrared")}>Infrared</button>
            <button onClick={() => handelApplyFilter("Old Television")}>Old Television</button>
            <button onClick={() => handelApplyFilter("Lighten")}>Lighten Brightness</button>
            <button onClick={() => handelApplyFilter("Darken")}>Darken Brightness</button>
            <button onClick={() => handelApplyFilter("Rotate")}>Rotate Image</button>
            <button onClick={() => handelApplyFilter("Sunny")}>Sunny</button>
            <button onClick={() => handelApplyFilter("Merge 2 Images")}>Merge 2 Images</button>
            <button onClick={() => handelApplyFilter("Resize")}>Resize</button>
            <button onClick={() => handelApplyFilter("Blur")}>Blur</button>
            <button onClick={() => handelApplyFilter("Edge Detection")}>Edge Detection</button>
            <button onClick={() => handelApplyFilter("Crop Image")}>Crop Image</button>
          </div>
        </div>
      </div>
    </>
  );
}

export default App;
