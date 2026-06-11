function resetProgress() {
    const progressBarContainer = document.getElementById("progress-bar-container");
    const progressBar = document.getElementById("progress-bar");
    const status = document.getElementById("status");

    status.textContent = "";
    progressBar.style.width = "0%";
    progressBarContainer.style.display = "none";
}

function uploadFile() {
    const fileInput = document.getElementById("file-input");
    const usernameInput = document.getElementById("username");
    const passwordInput = document.getElementById("password");

    const progressBarContainer = document.getElementById("progress-bar-container");
    const progressBar = document.getElementById("progress-bar");
    const status = document.getElementById("status");

    status.textContent = "";
    progressBar.style.width = "0%";
    progressBarContainer.style.display = "block";

    if (!fileInput.files[0]) {
        alert("Please select a file.");
        progressBarContainer.style.display = "none";
        return;
    }

    const formData = new FormData();
    formData.append("file", fileInput.files[0]);
    formData.append("username", usernameInput.value);
    formData.append("password", passwordInput.value);

    const xhr = new XMLHttpRequest();
    xhr.open("POST", "/upload", true);

    xhr.upload.onprogress = function (event) {
        if (event.lengthComputable) {
            const percentComplete = (event.loaded / event.total) * 100;
            progressBar.style.width = percentComplete + "%";
            status.textContent = `Uploading... ${Math.round(percentComplete)}%`;
        }
    };

    xhr.onload = function () {
        progressBarContainer.style.display = "none";

        if (xhr.status === 200) {
            status.textContent = "Upload Complete!";
        } else {
            status.textContent = "Upload Failed.";
        }
    };

    xhr.onerror = function () {
        progressBarContainer.style.display = "none";
        status.textContent = "Upload Error.";
    };

    xhr.send(formData);
}

document.getElementById("file-input").addEventListener("change", resetProgress);
