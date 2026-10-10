import { useState } from "react";
// import luffy from "../../luffy.jpg";
import FiltersMenu from "./menu-components/filters-menu";
import FlipMenu from "./menu-components/Flip-menu";
import BrightnessMenu from "./menu-components/Brightness-menu";
import BlurMenu from "./menu-components/Blur-menu";
import ResizeMenu from "./menu-components/Resize-menu";
import RotateMenu from "./menu-components/Rotate-menu";
import CropMenu from "./menu-components/Crop-menu";
import MergeMenu from "./menu-components/Merge-menu";
import FrameMenu from "./menu-components/Frame-menu";
import SkewMenu from "./menu-components/Skew-menu";
import ImageDisplay from "./Image-display";
function App() {
  const [menu, setMenu] = useState("Filters");
  const [dimensions, setDimensions] = useState({ width: 0, height: 0 });
  const [loading, setLoading] = useState(false);
  const [loadedImage, setLoadedImage] = useState("");
  const [filteredImage, setFilteredImage] = useState("");
  async function handleImageUpload() {
    try {
      setLoading(true);
      // setLoadedImage(luffy);
      // setFilteredImage(luffy);
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
    } finally {
      setLoading(false);
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
    } finally {
      setLoading(false);
    }
  }
  async function handelApplyFilter(filter, ...args) {
    try {
      setLoading(true);
      const status = await window.applyFilter(filter, ...args);
      if (status !== "SUCCESS") throw new Error("filter not applied");
      const base64String = await window.getImageBase64();
      setFilteredImage(`data:image/png;base64,${base64String}`);
      // filteredImageStatusRef.current.innerText = "Filter Successfully Applied!";
    } catch (error) {
      // filteredImageStatusRef.current.innerText = "An unexpected error occured.";
      console.log(error);
    } finally {
      setLoading(false);
    }
  }

  async function handleResetFilteredImage() {
    try {
      setLoading(true);
      const status = await window.resetFilteredImage();
      if (status !== "SUCCESS") throw new Error("image not reset");
      setFilteredImage(loadedImage);
    } catch (error) {
      console.log(error);
    } finally {
      setLoading(false);
    }
  }

  let component;
  switch (menu) {
    case "Filters":
      component = <FiltersMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} loading={loading} />;
      break;
    case "Flip":
      component = <FlipMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} loading={loading} />;
      break;
    case "Lighten":
    case "Darken":
      component = <BrightnessMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} type={menu} loading={loading} />;
      break;
    case "Blur":
      component = <BlurMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} loading={loading} dimensions={dimensions} />;
      break;
    case "Resize":
      component = <ResizeMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} loading={loading} />;
      break;
    case "Rotate":
      component = <RotateMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} loading={loading} />;
      break;
    case "Crop":
      component = <CropMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} loading={loading} dimensions={dimensions} />;
      break;
    case "Merge":
      component = (
        <MergeMenu
          handelApplyFilter={handelApplyFilter}
          changeMenu={setMenu}
          loading={loading}
          setLoading={setLoading}
          dimensions={dimensions}
        />
      );
      break;
    case "Frame":
      component = <FrameMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} loading={loading} />;
      break;
    case "Skew":
      component = <SkewMenu handelApplyFilter={handelApplyFilter} changeMenu={setMenu} loading={loading} />;
  }
  return (
    <>
      <div className="container">
        <ImageDisplay
          filteredImage={filteredImage}
          handleImageSave={handleImageSave}
          handleImageUpload={handleImageUpload}
          handleResetFilteredImage={handleResetFilteredImage}
          setDimensions={setDimensions}
          loading={loading}
        />
        <div className="menu">
          <h2>{menu}</h2>
          {component}
        </div>
      </div>
    </>
  );
}

export default App;
