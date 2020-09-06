import Logger from './logger';
import HomePage from './home_page';
import FirmwarePage from './firmware_page';
import ConfigPage from './config_page';
import Configuration from './configuration';

class App {
    constructor() {
        this.logger = new Logger();
        this.homePage = new HomePage();
        this.config = new Configuration();
        this.configPage = new ConfigPage();
        this.firmwarePage = new FirmwarePage();
    }

    Initialize() {
        this.logger.set_parent_div(document.getElementById('LOG'));
        this.logger.info('Test info');
        this.logger.warning('Test warning');
        this.logger.error('Test error');

        let appel = document.getElementById('APP');

        this.homePage.set_parent_div(appel);
        this.configPage.set_parent_div(appel);
        this.firmwarePage.set_parent_div(appel);

        this.GetConfig();

        this.HashHandler();
        window.addEventListener('hashchange', this.HashHandler, false);
    }

    SaveConfig() {
        let xhr = new XMLHttpRequest();
        xhr.onload = function() {
            if (xhr.readyState === xhr.DONE) {
                if (xhr.status === 200) {
                    this.logger.info(xhr.responseText);
                }
                else {
                    this.logger.error(xhr.status + " " + xhr.responseText);
                }
            }
        }
        xhr.onerror = function() { this.logger.error("Send error"); }
        xhr.onabort = function() { this.logger.warning("Send canceled"); }

        xhr.open("POST", "/config.json", true);
        xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
        xhr.send(this.config.fromPagetoString());
    }

    GetConfig() {
        let xhr = new XMLHttpRequest();
        xhr.onload = function() {
            if (xhr.readyState === xhr.DONE) {
                if (xhr.status === 200) {
                    this.config.fromString(xhr.responseText);
                }
            }
        };
        xhr.open("GET", "/config.json", true);
        xhr.send();
    }

    SendCmd(cmdID) {
        let xhr = new XMLHttpRequest();
        xhr.onload = function() {
            if (xhr.readyState === xhr.DONE) {
                if (xhr.status === 200) {
                    this.logger.info(xhr.responseText);
                }
                else {
                    this.logger.error(xhr.status + " " + xhr.responseText);
                }
            }
        }
        xhr.onerror = function() { this.logger.error("Send error"); }
        xhr.onabort = function() { this.logger.warning("Send canceled"); }

        let data = document.getElementById('CCode').value;
        let str = JSON.stringify({ "cmd": cmdID, "data": data });

        xhr.open("POST", "/cmd.json", true);
        xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
        xhr.send(str);
    }

    SetColor() { this.SendCmd(2); }

    TurnOff()  { this.SendCmd(1); }

    PClick(cdiv) {
        document.getElementById('CCode').value = cdiv.getAttribute("name");
        this.SendCmd(1);
    }

    ResetBoard() { this.SendCmd(0xFE); }

    swfC(ii) {
        if (!ii) return;
        if (!ii.files) return;
        let f = ii.files[0];
        if (!f) return;
        let d = document.getElementById('swf');
        if (d) {
            d.innerHTML = f.name + "<br/>size: " + f.size + " bytes";
        }
    }

    UpFW() {
        let fs = document.getElementById('fwf');
        if (!(fs && fs.files)) return;

        let f = fs.files[0];
        if (!f) return;

        let xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            if (xhr.readyState === xhr.DONE) {
                if (xhr.status === 200) {
                    this.logger.info(xhr.responseText);
                }
                else {
                    this.logger.error(xhr.status + " " + xhr.responseText);
                }
            }
        }

        var upp = document.getElementById('swi');
        xhr.upload.addEventListener("progress", function(ev) {
            if (ev.lengthComputable) {
                let percent = 100 * ev.loaded / ev.total | 0;
                upp.innerHTML = percent + "%";
            }
        });

        xhr.open("POST", "/update", true);
        xhr.send(f);
    }

    HashHandler() {
        if (location.hash === "#home") {
            app.homePage.render();
            return;
        }

        if (location.hash === "#config") {
            app.configPage.render();
            return;
        }

        if (location.hash === "#firmware") {
            app.firmwarePage.render();
            return;
        }

        // treat everything else like #home
        app.homePage.render();
    }
}

var app = new App();
app.Initialize();
