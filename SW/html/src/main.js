class App {
    constructor(logger) {
    }

    Initialize() {
        logger.set_parent_div(document.getElementById('LOG'));

        let appel = document.getElementById('APP');
        homePage.set_parent_div(appel);
        configPage.set_parent_div(appel);
        firmwarePage.set_parent_div(appel);

        this.GetConfig();

        this.HashHandler();
        window.addEventListener('hashchange', this.HashHandler, false);
    }

    SaveConfig() {
        let xhr = new XMLHttpRequest();
        xhr.onload = function() {
            if (xhr.readyState === xhr.DONE) {
                if (xhr.status === 200) {
                    logger.info(xhr.responseText);
                }
                else {
                    logger.error(xhr.status + " " + xhr.responseText);
                }
            }
        };
        xhr.onerror = function() { logger.error("Send error"); };
        xhr.onabort = function() { logger.warning("Send canceled"); };

        xhr.open("POST", "/config.json", true);
        xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
        xhr.send(configuration.fromPagetoString());
    }

    GetConfig() {
        let xhr = new XMLHttpRequest();
        xhr.onload = function() {
            if (xhr.readyState === xhr.DONE) {
                if (xhr.status === 200) {
                    configuration.fromString(xhr.responseText);
                }
            }
        };
        xhr.open("GET", "/config.json", true);
        xhr.send();
    }

    SendCmd(cmdID, data) {
        let xhr = new XMLHttpRequest();
        xhr.onload = function() {
            if (xhr.readyState === xhr.DONE) {
                if (xhr.status === 200) {
                    logger.info(xhr.responseText);
                }
                else {
                    logger.error(xhr.status + " " + xhr.responseText);
                }
            }
        };
        xhr.onerror = function() { logger.error("Send error"); };
        xhr.onabort = function() { logger.warning("Send canceled"); };

        let str = JSON.stringify({ "cmd": cmdID, "data": data });

        xhr.open("POST", "/cmd.json", true);
        xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
        xhr.send(str);
    }

    TurnOff() {
        this.SendCmd(1, 0);
    }
    SetLightColor(userColor) {
        let str = userColor.toString(16);
        document.getElementById('userColor').value = str;
        this.SendCmd(2, userColor);
    }
    SetColor() {
        let str = document.getElementById('userColor').value;
        let val = parseInt(str, 16);
        this.SendCmd(2, val);
    }
    SetIntensity() {
        let val = document.getElementById('userInt').valueAsNumber;
        this.SendCmd(3, val);
    }
    ResetBoard() {
        this.SendCmd(0xFE, 0);
    }

    UpFW() {
        let f = firmwarePage.GetSelectedFile();
        if (f == null) return;

        let xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            if (xhr.readyState === xhr.DONE) {
                if (xhr.status === 200) {
                    logger.info(xhr.responseText);
                }
                else {
                    logger.error(xhr.status + " " + xhr.responseText);
                }
                firmwarePage.EndUpload();
            }
        };

        xhr.upload.addEventListener("progress", function(ev) {
            if (ev.lengthComputable) {
                let percent = 100 * ev.loaded / ev.total | 0;
                firmwarePage.SetUploadProgress(percent);
            }
        });

        firmwarePage.BeginUpload();

        xhr.open("POST", "/update", true);
        xhr.send(f);
    }

    TogglePass(b, id) {
        let e = document.querySelector('#' + id);
        if (e == null) return;

        if (e.getAttribute('type') === 'text' ) {
            e.setAttribute('type', 'password');
            b.innerText = 'show';
        }
        else {
            e.setAttribute('type', 'text');
            b.innerText = 'hide';
        }
    }

    HashHandler() {
        if (location.hash === "#home") {
            homePage.render();
            return;
        }

        if (location.hash === "#config") {
            configPage.render();
            return;
        }

        if (location.hash === "#firmware") {
            firmwarePage.render();
            return;
        }

        // treat everything else like #home
        homePage.render();
    }
}

var logger = new Logger();
var homePage = new HomePage();
var configuration = new Configuration();
var configPage = new ConfigPage();
var app = new App();
app.Initialize();
