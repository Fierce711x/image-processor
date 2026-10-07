import { useState, useRef } from "react";
import "./App.css";

function App() {
  const [loadedImage, setLoadedImage] = useState("");
  const loadedImageStatusRef = useRef();
  const [filteredImage, setFilteredImage] = useState("");
  const filteredImageStatusRef = useRef();
  async function handleImageUpload() {
    try {
      loadedImageStatusRef.current.innerText = "Opening file picker...";
      const filePath = await window.openImagePickerDialog();

      if (filePath === "CANCELLED") {
        loadedImageStatusRef.current.innerText = "Selection cancelled.";
        return;
      }

      loadedImageStatusRef.current.innerText = `Loading: ${filePath}`;

      const base64Data = await window.loadImageWithStb(filePath);

      if (base64Data === "ERROR") {
        loadedImageStatusRef.current.innerText = "Error parsing image file with stb_image.";
        return;
      }

      setLoadedImage(`data:image/png;base64,${base64Data}`);
      setFilteredImage(`data:image/png;base64,${base64Data}`);
      loadedImageStatusRef.current.innerText = "Image successfully loaded!";
      filteredImageStatusRef.current.innerText = "Image successfully loaded!";
    } catch (error) {
      loadedImageStatusRef.current.innerText = "An unexpected error occurred.";
      console.error(error);
    }
  }
  async function handleImageSave() {
    try {
      const result = await window.saveActiveImageDialog();
      if (result === "CANCELLED") {
        throw Error("CANCELLED");
      } else if (result === "WRITE_FAILED") {
        throw Error("WRITE_FAILED");
      } else if (result === "SUCCESS") {
        console.log("SUCCESS");
      } else {
        throw Error("unexpected error");
      }
    } catch (error) {
      console.error(error);
    }
  }
  async function handelApplyFilter() {
    try {
      const status = await window.applyFilter();
      if (status !== "SUCCESS") throw Error("filter not applied");
      const base64String = await window.getImageBase64();
      setFilteredImage(`data:image/png;base64,${base64String}`);
      filteredImageStatusRef.current.innerText = "Filter Successfully Applied!";
    } catch (error) {
      filteredImageStatusRef.current.innerText = "An unexpected error occured.";
      console.log(error);
    }
  }
  return (
    <>
      <div class="container" style={{ display: "flex", justifyContent: "center", alignItems: "center", flexFlow: "column nowrap" }}>
        <h2>Apply C++ Image Filters</h2>
        <div style={{ display: "flex", justifyContent: "center", alignItems: "center" }}>
          <div>
            <p ref={loadedImageStatusRef}></p>
            <img src={loadedImage} width={500} height={500} />
            <button onClick={() => handleImageUpload()}>choose image</button>
          </div>
          <div>
            <p ref={filteredImageStatusRef}></p>
            <img src={filteredImage} alt="" width={500} height={500} />
            <button onClick={() => handelApplyFilter()}>apply filter</button>
          </div>
        </div>
        <button onClick={() => handleImageSave()}>save img</button>
      </div>
    </>
  );
}

export default App;
