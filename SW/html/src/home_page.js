class HomePage {
    constructor() {
        this.pdiv = null;
        this.statusInfo = null;
    }

    CreateSmallColorWheel(id) {
        let cRGB = ["FF0000","FF8000","FFFF00","80FF00","00FF00","00FF80","00FFFF","0080FF","0000FF","8000FF","FF00FF","FF0080"];
        let cRYB = ["FE2712","FC600A","FB9902","FCCC1A","FEFE33","B2D732","66B032","347C98","0247FE","4424D6","8601AF","C21460"];
        let cc = id==0 ? cRGB : cRYB;
        let s = '';
        for (let i = 0; i < cc.length; ++i) {
            s += '<div class="colorBox" onclick="app.SetLightColor(' + parseInt("0x" + cc[i]) + ')" style="background-color:#' + cc[i] + ';">&nbsp;</div>';
        }
        return s;
    }

    set_status_object(si) {
        this.statusInfo = si;
    }
    set_parent_div(parentDiv) {
        this.pdiv = parentDiv;
    }
    render() {
        if (!this.pdiv) return;

        let s = '<h3>Select color</h3>' +
            '<div class="srow">' +
                '<label class="cfgL" for="userColor">Color code in hex</label>' +
                '<input id="userColor" class="mLeft" type="text" value="" maxlength="8" size="8" />' +
            '</div>' +
            '<div class="srow mTop">' +
                '<button class="mTop"       onclick="app.SetColor()">Set</button>' +
                '<button class="mLeft mTop" onclick="app.TurnOff()">Off</button>' +
            '</div>' +
            '<h3>Intensity</h3>' +
            '<div class="srow mTop">' +
                '<input id="userInt" type="range" class="intSel" min="1" max="100" value="1" oninput="app.SetIntensity()" />' +
            '</div>' +
            '<h3>RGB and RYB</h3>' +
            '<div class="srow mTop">' + this.CreateSmallColorWheel(0) + '</div>' +
            '<div class="srow mTop">' + this.CreateSmallColorWheel(1) + '</div>';

        this.pdiv.innerHTML = s;

        this.updateInfo();
    }

    updateInfo() {
        let a = document.getElementById('userColor');
        if (a != null) {
            let val = this.statusInfo.get('currentColor');
            a.value = val.toString(16);
        }

        a = document.getElementById('userInt');
        if (a != null) {
            let val = this.statusInfo.get('currentIntensity');
            a.value = val;
        }
    }
}
