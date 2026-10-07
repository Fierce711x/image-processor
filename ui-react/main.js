let menu = document.querySelector(".menu");
let filter_btn = document.querySelector(".apply filter");
let save_btn = document.querySelector(".save");
let reset_btn = document.querySelector(".reset");
let image = document.querySelector("img");

renderMenu();

function renderMenu(){
    menu.innerHTML=`<button class="img_load">Load new image</button>
        <button class="apply filter">Apply Filter</button>
        <button class="save">Save image</button>
        <button class="reset">Reset image to orginal state</button>
        `;

    menu.addEventListener("click",(e)=>{
        if(e.target.className==="img_load"){
            menu.innerHTML=`
            <h2>Enter Image File</h2>
            <label for="img_file" class="file_label">Upload Image<label>
            <input type="file" accept=".jpg,.jpeg,.bmp" id="img_file">
            <button class="back">Back</button>
            `;
        }

        if(e.target.className==="back"){
            menu.innerHTML=`
            <button class="img_load">Load new image</button>
            <button class="apply filter">Apply Filter</button>
            <button class="save">Save image</button>
            <button class="reset">Reset image to orginal state</button>
            `;
        }

        if(e.target.className==="apply filter"){
            menu.innerHTML=`
            <button>Invert Colors</button>
            <button>Flip Horizontally</button>
            <button>Flip Vertically</button>
            <button>Grayscale</button>
            <button>Black & White</button>
            <button>Purple Tint</button>
            <button>Infrared</button>
            <button>Old Television</button>
            <button>Lighten Brightness</button>
            <button>Darken Brightness</button>
            <button>Rotate Image</button>
            <button>Sunny</button>
            <button>Merge 2 Images</button>
            <button>Resize</button>
            <button>Blur</button>
            <button>Edge Detection</button>
            <button>Crop Image</button>
            <button class="back">Back</button>
            `
        }
    })
}























