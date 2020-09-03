function openTab(evt, tabID) {
    var tabBody = document.getElementsByClassName('tabBody');
    for (let i = 0; i < tabBody.length; ++i) {
        tabBody[i].style.display = "none";
    }

    var tabLink = document.getElementsByClassName('tabLink');
    for (let i = 0; i < tabLink.length; ++i) {
        tabLink[i].className = tabLink[i].className.replace(" active", "");
    }

    document.getElementById(tabID).style.display = "block";
    evt.currentTarget.className += " active";
}

const names = ['version', 'name', 'ap1s', 'ap1p', 'ap2s', 'ap2p', 'ipAddr', 'ipMask', 'ipGateway', 'ipDNS'];
class ConfigMain {
    constructor() {
        this.initialize();
    }

    initialize() {
        for (let i = 0; i < names.length; ++i) {
            this[names[i]] = '';
        }
        this.version = 2;
    }

    fromString(jStr) {
        this.initialize();
        let cfg = JSON.parse(jStr);
        for (const [key, value] of Object.entries(cfg)) {
            if (this.hasOwnProperty(key)) {
                this[key] = value;
            }
        }
        this.toPage();
    }

    toPage() {
        for (const [key, value] of Object.entries(this)) {
            let a = document.getElementById('p' + key);
            if (a !== null) {
                a.value = value;
            }
        }
    }

    fromPagetoString() {
        for (const key of Object.keys(this)) {
            let a = document.getElementById('p' + key);
            if (a !== null) {
                this[key] = a.value;
            }
        }
        return JSON.stringify(this);
    }
}

var config = new ConfigMain();

function SaveConfig() {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (xhr.readyState === xhr.DONE) {
            if (xhr.status === 200) {
                ii.innerHTML = xhr.responseText;
            }
            else {
                ii.innerHTML = "Error " + xhr.status + " " + xhr.responseText;
            }
        }
    }
    xhr.onerror = function() { ii.innerHTML = "Send error"; }
    xhr.onabort = function() { ii.innerHTML = "Send canceled"; }

    xhr.open("POST", "/config.json", true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.send(config.fromPagetoString());
}

function GetConfig() {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (xhr.readyState === xhr.DONE) {
            if (xhr.status === 200) {
                config.fromString(xhr.responseText);
            }
        }
    };
    xhr.open("GET", "/config.json", true);
    xhr.send();
}

function SendCmd(cmdID) {
    var ii;
    if (cmdID == 0xF0 || cmdID == 0xF1) {
        ii = document.getElementById('csi');
    }
    else {
        ii = document.getElementById('sTxt');
    }

    var xhr = new XMLHttpRequest();

    xhr.onload = function() {
        if (xhr.readyState === xhr.DONE) {
            if (xhr.status === 200) {
                ii.innerHTML = xhr.responseText;
            }
            else {
                ii.innerHTML = "Error " + xhr.status + " " + xhr.responseText;
            }
        }
    }
    xhr.onerror = function() { ii.innerHTML = "Send error"; }
    xhr.onabort = function() { ii.innerHTML = "Send canceled"; }

    var str;
    if (cmdID == 0xF0 || cmdID == 0xF1) {
        var ssid = document.getElementById('SSID').value;
        var pass = document.getElementById('PASS').value;
        str = JSON.stringify({ "cmd": cmdID, "ssid": ssid, "pass": pass });
    }
    else {
        var data = document.getElementById('CCode').value;
        str = JSON.stringify({ "cmd": cmdID, "data": data });
    }

    xhr.open("POST", "/cmd.json", true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.send(str);
}

function SetColor() { SendCmd(2); }

function TurnOff()  { SendCmd(1); }

function PClick(cdiv) {
    document.getElementById('CCode').value = cdiv.getAttribute("name");
    SendCmd(1);
}

function ResetBoard() { SendCmd(0xFE); }

var nRGB = ["800000","804000","808000","408000","008000","008040","008080","004080","000080","400080","800080","800040"];
var nRYB = ["7F1208","7F2F05","7C4B01","7F660C","7F7F19","596B18","325718","1A3E4C","01227F","22126B","440159","600A2F"];
var pRGB = ["FF0000","FF8000","FFFF00","80FF00","00FF00","00FF80","00FFFF","0080FF","0000FF","8000FF","FF00FF","FF0080"];
var pRYB = ["FE2712","FC600A","FB9902","FCCC1A","FEFE33","B2D732","66B032","347C98","0247FE","4424D6","8601AF","C21460"];
function CreateSmallColorWheel(id, parentID) {
    var mdiv = document.getElementById(parentID);
    var nn = id==0 ? nRGB : nRYB;
    var pp = id==0 ? pRGB : pRYB;
    var i, rdiv;
    for (i = 0; i < pp.length; i++) {
        if ((i % 6) == 0) {
            rdiv = document.createElement("div");
            rdiv.setAttribute("class", "btnr");
            mdiv.appendChild(rdiv);
        }
        rdiv.innerHTML += '<div class="btnc" name="' + nn[i] + '" onclick="PClick(this)" style="background-color:#' + pp[i] +';"></div>';
    }
}

function swfC(ii) {
    if (!(ii && ii.files)) return;
    var f = ii.files[0];
    if (!f) return;
    var d = document.getElementById('swf');
    if (d) {
        d.innerHTML = f.name + "<br/>size: " + f.size + " bytes";
    }
}

function UpFW() {
    var fs = document.getElementById('fwf');
    if (!(fs && fs.files)) return;

    var f = fs.files[0];
    if (!f) return;

    var ii = document.getElementById('swi');

    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState === xhr.DONE) {
            if (xhr.status === 200) {
                ii.innerHTML = xhr.responseText;
            }
            else {
                ii.innerHTML = "Error " + xhr.status + " " + xhr.responseText;
            }
        }
    }

    xhr.upload.addEventListener("progress", function(ev) {
        if (ev.lengthComputable) {
            let percent = 100 * ev.loaded / ev.total | 0;
            ii.innerHTML = percent + "%";
        }
    });

    xhr.open("POST", "/update", true);
    xhr.send(f);
}

function Init() {
    CreateSmallColorWheel(0, 'pRGB');
    CreateSmallColorWheel(1, 'pRYB');
    GetConfig();
}
