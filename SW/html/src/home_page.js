class HomePage {
    constructor() {
        this.pdiv = null;
    }

    CreateSmallColorWheel(id) {
        let cRGB = ["FF0000","FF8000","FFFF00","80FF00","00FF00","00FF80","00FFFF","0080FF","0000FF","8000FF","FF00FF","FF0080"];
        let cRYB = ["FE2712","FC600A","FB9902","FCCC1A","FEFE33","B2D732","66B032","347C98","0247FE","4424D6","8601AF","C21460"];
        let cc = id==0 ? cRGB : cRYB;
        let str = '';
        for ( let i = 0; i < cc.length; ++i) {
            if ((i % 6) == 0) {
                if (i != 0) {
                    str += '</div>';
                }
                str += '<div class="srow">';
            }
            str += '<div class="scell colorBox" onclick="app.SetLightColor(' + parseInt("0x" + cc[i]) + ')" style="background-color:#' + cc[i] +';">&nbsp;</div>';
        }
        str += '</div>';
        return str;
    }

    set_parent_div(parentDiv) {
        this.pdiv = parentDiv;
    }
    render() {
        if (!this.pdiv) return;

        let str = '';

        str = str + '<h3>Enter color code in hex</h3>';
        str = str + '<input id="userColor" type="text" value="" maxlength="8" size="8" />';
        str = str + '<button class="mrgLeft" onclick="app.SetColor()">Set</button>';
        str = str + '<button class="mrgLeft" onclick="app.TurnOff()">Off</button>';
        str = str + '<h3>Intensity</h3><div><input id="userInt" type="range" class="intSel" min="1" max="100" value="50" oninput="app.SetIntensity()" /></div><br/>';
        str = str + '<h3>RGB</h3><div>' + this.CreateSmallColorWheel(0) + '</div><br/>';
        str = str + '<h3>RYB</h3><div>' + this.CreateSmallColorWheel(1) + '</div>';

        this.pdiv.innerHTML = str;
    }
}
