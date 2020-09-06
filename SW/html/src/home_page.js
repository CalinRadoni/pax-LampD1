class HomePage {
    constructor() {
        this.pdiv = null;
    }

    CreateSmallColorWheel(id) {
        let nRGB = ["800000","804000","808000","408000","008000","008040","008080","004080","000080","400080","800080","800040"];
        let nRYB = ["7F1208","7F2F05","7C4B01","7F660C","7F7F19","596B18","325718","1A3E4C","01227F","22126B","440159","600A2F"];
        let pRGB = ["FF0000","FF8000","FFFF00","80FF00","00FF00","00FF80","00FFFF","0080FF","0000FF","8000FF","FF00FF","FF0080"];
        let pRYB = ["FE2712","FC600A","FB9902","FCCC1A","FEFE33","B2D732","66B032","347C98","0247FE","4424D6","8601AF","C21460"];
        let nn = id==0 ? nRGB : nRYB;
        let pp = id==0 ? pRGB : pRYB;
        let str = '';
        for ( let i = 0; i < pp.length; i++) {
            if ((i % 6) == 0) {
                if (i != 0) {
                    str += '</div>';
                }
                str += '<div class="btnr">';
            }
            str += '<div class="btnc" name="' + nn[i] + '" onclick="PClick(this)" style="background-color:#' + pp[i] +';"></div>';
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

        str = str + '<h3>AAA Enter color code in hex</h3>';
        str = str + '<input id="CCode" type="text" value="" maxlength="8" size="8">&nbsp';
        str = str + '<input type="button" value="Set" onclick="SetColor()" />&nbsp';
        str = str + '<input type="button" value="Off" onclick="TurnOff()" /><br/>';
        str = str + '<h3>RGB</h3><div>' + this.CreateSmallColorWheel(0) + '</div><br/>';
        str = str + '<h3>RYB</h3><div>' + this.CreateSmallColorWheel(1) + '</div>';

        this.pdiv.innerHTML = str;
    }
};

export default HomePage;
